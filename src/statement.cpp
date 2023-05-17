/**
 * @file   statement.cpp
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/errors/sqlite.h>
#include <sqlite-burrito/statement.h>

#include <iterator>

#include <sqlite3.h>

using namespace sqlite_burrito;

statement::statement(connection &conn, prepare_flags flags)
   : connection_{&conn}
   , flags_{flags} {
   // Nothing to do here
}

statement::~statement() {
   ::sqlite3_finalize(stmt_);
}

statement::iterator_t statement::prepare(std::string_view text) {
   std::error_code ec;
   auto res = prepare(text, ec);
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

statement::iterator_t statement::prepare(std::string_view text, std::error_code &ec) noexcept {
   native_handle_t new_statement{nullptr};

   const char *tail = nullptr;
   auto res = ::sqlite3_prepare_v3(&connection_->native_handle(), text.data(), static_cast<int>(text.length()),
                                   static_cast<unsigned>(flags_), &new_statement, &tail);
   ec = errors::make_error_code(res);

   if (!ec && new_statement) {
      ::sqlite3_finalize(stmt_);
      stmt_ = new_statement;
   }

   auto distance = std::distance(text.data(), tail);

   if (distance < text.length()) {
      return text.begin() + distance;
   } else {
      return std::end(text);
   }
}

statement::optional_str_t statement::execute(connection &conn, std::string_view sql, exec_callback_t callback) {
   std::error_code ec;
   auto res = execute(conn, sql, ec, std::move(callback));
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

statement::optional_str_t statement::execute(connection &conn,
                                             std::string_view sql,
                                             std::error_code &ec,
                                             exec_callback_t callback) {
   exec_callback_wrapper wrapper{std::move(callback)};
   char *error_message{nullptr};

   auto res = ::sqlite3_exec(&conn.native_handle(), sql.data(), &exec_callback_wrapper::call, &wrapper, &error_message);
   ec = errors::make_error_code(res);

   if (error_message != nullptr) {
      std::string msg{error_message};
      sqlite3_free(error_message);
      return msg;
   }

   return std::nullopt;
}

statement::optional_str_t statement::execute(connection &conn, std::string_view sql) {
   std::error_code ec;
   auto res = execute(conn, sql, ec);
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

statement::optional_str_t statement::execute(connection &conn, std::string_view sql, std::error_code &ec) {
   char *error_message{nullptr};

   auto res = ::sqlite3_exec(&conn.native_handle(), sql.data(), nullptr, nullptr, &error_message);
   ec = errors::make_error_code(res);

   if (error_message != nullptr) {
      std::string msg{error_message};
      sqlite3_free(error_message);
      return msg;
   }

   return std::nullopt;
}

void statement::reset() {
   std::error_code ec;
   reset(ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void statement::reset(std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_reset(stmt_));
}

bool statement::step() {
   std::error_code ec;
   bool result = step(ec);

   if (result) {
      return true;
   }

   if (ec == errors::condition::done) {
      // Statement is done - no more data available, but it is still not an error
      return false;
   }

   if (ec) {
      // Neither a row is available, nor was the step finished: it's an error
      throw std::system_error(ec);
   } else {
      return false;
   }
}

bool statement::step(std::error_code ec) {
   ec = errors::make_error_code(::sqlite3_step(stmt_));
   return (ec == errors::condition::row);
}

int statement::execute() {
   std::error_code ec;
   auto result = execute(ec);

   if (ec && ec != errors::condition::done && ec != errors::condition::row) {
      throw std::system_error(ec);
   }

   return result;
}

int statement::execute(std::error_code &ec) {
   step(ec);

   if (ec == errors::condition::done) {
      return ::sqlite3_changes(&connection_->native_handle());
   }

   return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Index-based binds
////////////////////////////////////////////////////////////////////////////////
void statement::bind_null(int index) {
   std::error_code ec;
   bind_null(index, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void statement::bind(int index, const void *blob, std::size_t size) {
   std::error_code ec;
   bind(index, blob, size, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void statement::fill_parameters_map() {
   if (parameters_) {
      // Already filled up
      return;
   }

   // Construct default map
   parameters_ = decltype(parameters_)::value_type{};
   auto &map = parameters_.value();

   // Fill the map
   auto max_index = ::sqlite3_bind_parameter_count(stmt_);
   for (int i = 1; i <= max_index; ++i) {
      auto ptr = sqlite3_bind_parameter_name(stmt_, i);
      if (!ptr) {
         // Either out of range, or a nameless parameter
         continue;
      }

      map[ptr] = i;
   }
}

std::optional<int> statement::find_parameter_by_name(std::string_view name) {
   if (!parameters_) {
      fill_parameters_map();
   }

   auto it = parameters_->find(name);
   if (it == parameters_->end()) {
      return {};
   }

   return it->second;
}

void statement::bind_null(int index, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_null(stmt_, index));
}

void statement::bind(int index, double value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_double(stmt_, index, value));
}

void statement::bind(int index, std::int8_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, value));
}

void statement::bind(int index, std::int16_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, value));
}

void statement::bind(int index, std::int32_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, value));
}

void statement::bind(int index, std::int64_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int64(stmt_, index, value));
}

void statement::bind(int index, std::uint8_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, value));
}

void statement::bind(int index, std::uint16_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, value));
}

void statement::bind(int index, std::uint32_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int(stmt_, index, static_cast<std::int32_t>(value)));
}

void statement::bind(int index, std::uint64_t value, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_int64(stmt_, index, static_cast<std::int64_t>(value)));
}

void statement::bind(int index, std::string_view value, std::error_code &ec) {
   ec = errors::make_error_code(
       ::sqlite3_bind_text(stmt_, index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT));
}

void statement::bind(int index, const void *blob, std::size_t size, std::error_code &ec) {
   ec = errors::make_error_code(::sqlite3_bind_blob64(stmt_, index, blob, size, SQLITE_TRANSIENT));
}

////////////////////////////////////////////////////////////////////////////////
/// Name-based binds
////////////////////////////////////////////////////////////////////////////////
void statement::bind_null(std::string_view name) {
   std::error_code ec;
   bind_null(name, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void statement::bind_null(std::string_view name, std::error_code &ec) {
   auto optional_idx = find_parameter_by_name(name);
   if (!optional_idx) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   bind_null(*optional_idx, ec);
}

////////////////////////////////////////////////////////////////////////////////
/// Column information
////////////////////////////////////////////////////////////////////////////////
bool statement::is_int(int index) {
   return sqlite3_column_type(stmt_, index) == SQLITE_INTEGER;
}

bool statement::is_float(int index) {
   return sqlite3_column_type(stmt_, index) == SQLITE_FLOAT;
}

bool statement::is_text(int index) {
   return sqlite3_column_type(stmt_, index) == SQLITE_TEXT;
}

bool statement::is_blob(int index) {
   return sqlite3_column_type(stmt_, index) == SQLITE_BLOB;
}

bool statement::is_null(int index) {
   return sqlite3_column_type(stmt_, index) == SQLITE_NULL;
}

////////////////////////////////////////////////////////////////////////////////
/// Index-based getters
////////////////////////////////////////////////////////////////////////////////
void statement::get(int index, double &value, std::error_code &ec) {
   value = ::sqlite3_column_double(stmt_, index);
   ec = connection_->last_error();
}

void statement::get(int index, std::int8_t &value, std::error_code &ec) {
   value = static_cast<std::int8_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::int16_t &value, std::error_code &ec) {
   value = static_cast<std::int16_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::int32_t &value, std::error_code &ec) {
   value = static_cast<std::int32_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::int64_t &value, std::error_code &ec) {
   value = ::sqlite3_column_int64(stmt_, index);
   ec = connection_->last_error();
}

void statement::get(int index, std::uint8_t &value, std::error_code &ec) {
   value = static_cast<std::uint8_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::uint16_t &value, std::error_code &ec) {
   value = static_cast<std::uint16_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::uint32_t &value, std::error_code &ec) {
   value = static_cast<std::uint32_t>(::sqlite3_column_int(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::uint64_t &value, std::error_code &ec) {
   value = static_cast<std::uint64_t>(::sqlite3_column_int64(stmt_, index));
   ec = connection_->last_error();
}

void statement::get(int index, std::string &value, std::error_code &ec) {
   using value_type = std::string::value_type;

   auto num_bytes = static_cast<std::size_t>(::sqlite3_column_bytes(stmt_, index));
   if (!num_bytes) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   auto num_chars = num_bytes / sizeof(value_type);
   if (num_chars * sizeof(value_type) < num_bytes) {
      ++num_chars;
   }

   value.resize(num_chars);

   auto from = reinterpret_cast<const char *>(::sqlite3_column_text(stmt_, index));
   std::copy(from, from + num_bytes, std::begin(value));
}

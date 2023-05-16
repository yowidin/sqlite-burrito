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
   , flags_{flags}
   , stmt_{nullptr} {
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
   const char *tail = nullptr;
   auto res = ::sqlite3_prepare_v3(&connection_->native_handle(), text.data(), static_cast<int>(text.length()),
                                   static_cast<unsigned>(flags_), &stmt_, &tail);
   ec = errors::make_error_code(res);

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
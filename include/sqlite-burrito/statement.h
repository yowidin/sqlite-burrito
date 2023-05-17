/**
 * @file   statement.h
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_STATEMENT_H
#define INCLUDE_SQLITE_BURRITO_STATEMENT_H

#include <sqlite-burrito/config.h>
#include <sqlite-burrito/errors/sqlite.h>
#include <sqlite-burrito/export.h>

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace sqlite_burrito {

class connection;

SQLITE_BURRITO_EXPORT class statement {
public:
   enum class prepare_flags : unsigned {
      //! No additional flags
      default_flags = 0,

      //! A hint to the query planner that the prepared statement will be
      //! retained for a long time and probably reused many times. Without this
      //! flag, the SQLite prepare* functions assume that the prepared statement
      //! will be used just once or at most a few times and then destroyed
      //! relatively soon. The current implementation acts on this hint by
      //! avoiding the use of lookaside memory so as not to deplete the limited
      //! store of lookaside memory. Future versions of SQLite may act on this
      //! hint differently.
      persistent = SQLITE_PREPARE_PERSISTENT,

      //! Causes the SQL compiler to return an error if the statement uses any
      //! virtual tables
      no_vtab = SQLITE_PREPARE_NO_VTAB
   };

   //! Type alias for the string iterator
   using iterator_t = std::string_view::const_iterator;

   using exec_callback_t = std::function<bool(int num, char **values, char **names)>;

   using native_handle_t = ::sqlite3_stmt *;

   using optional_str_t = std::optional<std::string>;

private:
   struct exec_callback_wrapper {
      explicit exec_callback_wrapper(exec_callback_t cb)
         : cb_{std::move(cb)} {
         // Nothing to do here
      }

      static int call(void *ptr, int num, char **values, char **names) {
         auto instance = reinterpret_cast<exec_callback_wrapper *>(ptr);
         return instance->cb_(num, values, names) ? 1 : 0;
      }

      exec_callback_t cb_;
   };

public:
   explicit statement(connection &conn, prepare_flags flags = prepare_flags::default_flags);

   statement(statement &) = delete;
   statement(statement &&) = default;

   ~statement();

public:
   statement &operator=(statement &) = delete;
   statement &operator=(statement &&) = default;

public:
   /**
    * Prepare an SQL statement.
    * This function only compiles the first statement in text, so return value is pointing to what remains un-compiled.
    * @param text Statement to be compiled
    * @return Iterator pointing to the first byte past the end of the first SQL statement in `text`.
    */
   iterator_t prepare(std::string_view text);

   /**
    * Prepare an SQL statement.
    * This function only compiles the first statement in text, so return value is pointing to what remains un-compiled.
    * @param text Statement to be compiled
    * @param ec Error code, storing the operation result.
    * @return Iterator pointing to the first byte past the end of the first SQL statement in `text`.
    */
   iterator_t prepare(std::string_view text, std::error_code &ec) noexcept;

   [[nodiscard]] auto &native_handle() { return *stmt_; };
   [[nodiscard]] const auto &native_handle() const { return *stmt_; }

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param callback Callback to be invoked for each result row coming out of the evaluated SQL statements
    * @return Optional string, containing an error message in case if one of the requests is failed
    */
   static optional_str_t execute(connection &conn, std::string_view sql, exec_callback_t callback);

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param ec Error code, storing the operation result.
    * @param callback Callback to be invoked for each result row coming out of the evaluated SQL statements
    * @return Error message in case if one of the requests is failed, an empty string otherwise
    */
   static optional_str_t execute(connection &conn, std::string_view sql, std::error_code &ec, exec_callback_t callback);

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @return Error message in case if one of the requests is failed, an empty string otherwise
    */
   static optional_str_t execute(connection &conn, std::string_view sql);

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param ec Error code, storing the operation result.
    * @return Error message in case if one of the requests is failed, an empty string otherwise
    */
   static optional_str_t execute(connection &conn, std::string_view sql, std::error_code &ec);

   void reset();
   void reset(std::error_code &ec);

   /**
    * Execute the a `step` call on a prepared statement.
    * Throws an exception in case of an error
    * @return true if there is a row available (can be accessed via `get`)
    */
   bool step();

   /**
    * Execute the a `step` call on a prepared statement.
    * @param ec Error code to store the step result in.
    * @return true if there is a row available (can be accessed via `get`)
    */
   bool step(std::error_code ec);

   /**
    * @return Number of rows modified by this statement
    */
   int execute();

   /**
    * @return Number of rows modified by this statement
    */
   int execute(std::error_code &ec);

   ////////////////////////////////////////////////////////////////////////////////
   /// Index-based binds
   ////////////////////////////////////////////////////////////////////////////////
   void bind_null(int index);
   void bind_null(int index, std::error_code &ec);

   void bind(int index, const void *blob, std::size_t size);
   void bind(int index, const void *blob, std::size_t size, std::error_code &ec);

   template <typename T>
   void bind(int index, T value);

   void bind(int index, double value, std::error_code &ec);
   void bind(int index, std::int8_t value, std::error_code &ec);
   void bind(int index, std::int16_t value, std::error_code &ec);
   void bind(int index, std::int32_t value, std::error_code &ec);
   void bind(int index, std::int64_t value, std::error_code &ec);
   void bind(int index, std::uint8_t value, std::error_code &ec);
   void bind(int index, std::uint16_t value, std::error_code &ec);
   void bind(int index, std::uint32_t value, std::error_code &ec);
   void bind(int index, std::uint64_t value, std::error_code &ec);
   void bind(int index, std::string_view value, std::error_code &ec);

   template <typename T>
   void bind(int index, const std::vector<T> &value, std::error_code &ec);

   template <typename T, std::size_t N>
   void bind(int index, const std::array<T, N> &value, std::error_code &ec);

   template <typename T, std::size_t N>
   void bind(int index, const T (&value)[N]);

   template <typename T, std::size_t N>
   void bind(int index, const T (&value)[N], std::error_code &ec);

   template <typename T>
   void bind(int index, const std::optional<T> &value, std::error_code &ec);

   ////////////////////////////////////////////////////////////////////////////////
   /// Name-based binds
   ////////////////////////////////////////////////////////////////////////////////
   void bind_null(std::string_view name);
   void bind_null(std::string_view name, std::error_code &ec);

   template <typename T>
   void bind(std::string_view name, const T &value);

   template <typename T>
   void bind(std::string_view name, const T &value, std::error_code &ec);

   template <typename T, std::size_t N>
   void bind(std::string_view name, const T (&value)[N]);

   template <typename T, std::size_t N>
   void bind(std::string_view name, const T (&value)[N], std::error_code &ec);

   ////////////////////////////////////////////////////////////////////////////////
   /// Column information
   ////////////////////////////////////////////////////////////////////////////////
   bool is_int(int index);
   bool is_float(int index);
   bool is_text(int index);
   bool is_blob(int index);
   bool is_null(int index);

   ////////////////////////////////////////////////////////////////////////////////
   /// Index-based getters
   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   void get(int index, T &value);

   void get(int index, float &value, std::error_code &ec);
   void get(int index, double &value, std::error_code &ec);
   void get(int index, std::int8_t &value, std::error_code &ec);
   void get(int index, std::int16_t &value, std::error_code &ec);
   void get(int index, std::int32_t &value, std::error_code &ec);
   void get(int index, std::int64_t &value, std::error_code &ec);
   void get(int index, std::uint8_t &value, std::error_code &ec);
   void get(int index, std::uint16_t &value, std::error_code &ec);
   void get(int index, std::uint32_t &value, std::error_code &ec);
   void get(int index, std::uint64_t &value, std::error_code &ec);
   void get(int index, std::string &value, std::error_code &ec);

   template <typename T>
   void get(int index, std::vector<T> &value, std::error_code &ec);

   template <typename T, std::size_t N>
   void get(int index, std::array<T, N> &value, std::error_code &ec);

   template <typename T, std::size_t N>
   void get(int index, T (&value)[N]);

   template <typename T, std::size_t N>
   void get(int index, T (&value)[N], std::error_code &ec);

   template <typename T>
   void get(int index, std::optional<T> &value, std::error_code &ec);

private:
   void fill_parameters_map();

   std::optional<int> find_parameter_by_name(std::string_view name);

private:
   //! Database connection, this statement belongs to
   connection *connection_;

   //! Additional flags to the prepare command
   prepare_flags flags_;

   //! Native handle
   native_handle_t stmt_{nullptr};

   //! Named parameters map
   std::optional<std::map<std::string_view, int>> parameters_{};
};

////////////////////////////////////////////////////////////////////////////////
/// Index-based binds
////////////////////////////////////////////////////////////////////////////////
template <typename T>
void statement::bind(int index, T value) {
   std::error_code ec;
   bind(index, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T>
void statement::bind(int index, const std::vector<T> &value, std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");
   const auto full_size = sizeof(T) * value.size();
   bind(index, value.data(), full_size, ec);
}

template <typename T, std::size_t N>
void statement::bind(int index, const std::array<T, N> &value, std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");
   const auto full_size = sizeof(T) * value.size();
   bind(index, value.data(), full_size, ec);
}

template <typename T, std::size_t N>
void statement::bind(int index, const T (&value)[N]) {
   std::error_code ec;
   bind(index, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T, std::size_t N>
void statement::bind(int index, const T (&value)[N], std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");
   const auto full_size = sizeof(T) * N;
   bind(index, value, full_size, ec);
}

template <typename T>
void statement::bind(int index, const std::optional<T> &value, std::error_code &ec) {
   if (value) {
      bind(index, *value, ec);
   } else {
      bind_null(index, ec);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Name-based binds
////////////////////////////////////////////////////////////////////////////////
template <typename T>
void statement::bind(std::string_view name, const T &value) {
   std::error_code ec;
   bind(name, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T>
void statement::bind(std::string_view name, const T &value, std::error_code &ec) {
   auto optional_idx = find_parameter_by_name(name);
   if (!optional_idx) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   bind(*optional_idx, value, ec);
}

template <typename T, std::size_t N>
void statement::bind(std::string_view name, const T (&value)[N]) {
   std::error_code ec;
   bind(name, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T, std::size_t N>
void statement::bind(std::string_view name, const T (&value)[N], std::error_code &ec) {
   auto optional_idx = find_parameter_by_name(name);
   if (!optional_idx) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   bind(*optional_idx, value, ec);
}

////////////////////////////////////////////////////////////////////////////////
/// Index-based getters
////////////////////////////////////////////////////////////////////////////////
template <typename T>
void statement::get(int index, T &value) {
   std::error_code ec;
   get(index, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T>
void statement::get(int index, std::vector<T> &value, std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");

   auto num_bytes = static_cast<std::size_t>(::sqlite3_column_bytes(stmt_, index));
   if (!num_bytes) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   auto num_values = num_bytes / sizeof(T);
   if (num_values * sizeof(T) < num_bytes) {
      ++num_values;
   }

   value.clear();
   value.resize(num_values);

   auto from = reinterpret_cast<const std::byte *>(::sqlite3_column_blob(stmt_, index));
   if (!from) {
      ec = connection_->last_error();
      if (ec == errors::condition::row) {
         // It's actually a NULL, and not an error. But we don't allow it here
         ec = std::make_error_code(std::errc::invalid_argument);
      }
      return;
   }

   auto to = reinterpret_cast<std::byte *>(value.data());
   std::copy(from, from + num_bytes, to);
}

template <typename T, std::size_t N>
void statement::get(int index, std::array<T, N> &value, std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");

   auto num_bytes = static_cast<std::size_t>(::sqlite3_column_bytes(stmt_, index));
   if (!num_bytes) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   auto num_values = num_bytes / sizeof(T);
   if (num_values * sizeof(T) < num_bytes) {
      ++num_values;
   }

   if (num_values != N) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   auto from = reinterpret_cast<const std::byte *>(::sqlite3_column_blob(stmt_, index));
   if (!from) {
      ec = connection_->last_error();
      if (ec == errors::condition::row) {
         // It's actually a NULL, and not an error. But we don't allow it here
         ec = std::make_error_code(std::errc::invalid_argument);
      }
      return;
   }

   auto to = reinterpret_cast<std::byte *>(value.data());
   std::copy(from, from + num_bytes, to);
}

template <typename T, std::size_t N>
void statement::get(int index, T (&value)[N]) {
   std::error_code ec;
   get(index, value, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

template <typename T, std::size_t N>
void statement::get(int index, T (&value)[N], std::error_code &ec) {
   static_assert(std::is_pod_v<T>, "POD type expected");

   auto num_bytes = static_cast<std::size_t>(::sqlite3_column_bytes(stmt_, index));
   if (!num_bytes) {
      ec = errors::make_error_code(errors::code::constraint_notnull);
      return;
   }

   auto num_values = num_bytes / sizeof(T);
   if (num_values * sizeof(T) < num_bytes) {
      ++num_values;
   }

   if (num_values != N) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   auto from = reinterpret_cast<const std::byte *>(::sqlite3_column_blob(stmt_, index));
   if (!from) {
      ec = connection_->last_error();
      if (ec == errors::condition::row) {
         // It's actually a NULL, and not an error. But we don't allow it here
         ec = std::make_error_code(std::errc::invalid_argument);
      }
      return;
   }

   auto to = reinterpret_cast<std::byte *>(value);
   std::copy(from, from + num_bytes, to);
}

template <typename T>
void statement::get(int index, std::optional<T> &value, std::error_code &ec) {
   if (is_null(index)) {
      value = std::nullopt;
   } else {
      T result;
      get(index, result, ec);
      value = std::move(result);
   }
}

} // namespace sqlite_burrito

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::statement::prepare_flags operator|(
    sqlite_burrito::statement::prepare_flags lhs,
    sqlite_burrito::statement::prepare_flags rhs) {
   return static_cast<sqlite_burrito::statement::prepare_flags>(static_cast<unsigned>(lhs) |
                                                                static_cast<unsigned>(rhs));
}

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::statement::prepare_flags &operator|=(
    sqlite_burrito::statement::prepare_flags &lhs,
    sqlite_burrito::statement::prepare_flags rhs) {
   lhs = static_cast<sqlite_burrito::statement::prepare_flags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
   return lhs;
}

#endif /* INCLUDE_SQLITE_BURRITO_STATEMENT_H */

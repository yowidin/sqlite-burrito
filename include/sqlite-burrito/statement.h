/**
 * @file   statement.h
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_STATEMENT_H
#define INCLUDE_SQLITE_BURRITO_STATEMENT_H

#include <sqlite-burrito/config.h>
#include <sqlite-burrito/export.h>
#include <sqlite-burrito/isolation/sqlite3.h>

#include <memory>
#include <string>
#include <system_error>
#include <functional>

namespace sqlite_burrito {

namespace detail {
class statement_ptr;
}

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
      persistent = isolation::sqlite_prepare_persistent,

      //! Causes the SQL compiler to return an error if the statement uses any
      //! virtual tables
      no_vtab = isolation::sqlite_prepare_no_vtab
   };

   //! Type alias for the string iterator
   using iterator_t = std::string::const_iterator;

   using exec_callback_t = std::function<bool(int, char **, char**)>;

private:
   struct exec_callback_wrapper {
      explicit exec_callback_wrapper(exec_callback_t cb)
         : cb_{std::move(cb)} {}

      static int call(void *ptr, int num, char **values, char **names) {
         auto instance = reinterpret_cast<exec_callback_wrapper *>(ptr);
         return instance->cb_(num, values, names) ? 1 : 0;
      }

      exec_callback_t cb_;
   };

public:
   explicit statement(connection &conn,
                      prepare_flags flags = prepare_flags::default_flags);

   statement(statement &) = delete;
   statement(statement &&) = default;

   ~statement();

public:
   statement &operator=(statement &) = delete;
   statement &operator=(statement &&) = default;

public:
   /**
    * Prepare an SQL statement.
    * This function only compiles the first statement in text, so return value
    * is pointing to what remains uncompiled.
    * @param text Statement to be compiled
    * @return Iterator pointing to the first byte past the end of the first SQL
    * statement in `text`.
    */
   iterator_t prepare(const std::string &text);

   /**
    * Prepare an SQL statement.
    * This function only compiles the first statement in text, so return value
    * is pointing to what remains uncompiled.
    * @param text Statement to be compiled
    * @param ec Error code, storing the operation result.
    * @return Iterator pointing to the first byte past the end of the first SQL
    * statement in `text`.
    */
   iterator_t prepare(const std::string &text, std::error_code &ec) noexcept;

   detail::statement_ptr &get_native_wrapper() { return *stmt_; };
   detail::statement_ptr &get_native_wrapper() const { return *stmt_; }

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param ec Error code, storing the operation result.
    * @param callback Callback to be invoked for each result row coming out of
    * the evaluated SQL statements
    * @return Error message in case if one of the requests is failed, an empty
    * string otherwise
    */
   static std::string execute(
       connection &conn,
       const char *sql,
       std::error_code &ec,
       exec_callback_t callback) noexcept(noexcept(std::string()));

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param callback Callback to be invoked for each result row coming out of
    * the evaluated SQL statements
    * @return Error message in case if one of the requests is failed, an empty
    * string otherwise
    */
   static std::string execute(connection &conn,
                              const char *sql,
                              exec_callback_t callback);

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @param ec Error code, storing the operation result.
    * @return Error message in case if one of the requests is failed, an empty
    * string otherwise
    */
   static std::string execute(
       connection &conn,
       const char *sql,
       std::error_code &ec) noexcept(noexcept(std::string()));

   /**
    * Runs zero or more UTF-8 encoded, semicolon-separate SQL statements
    * @param conn Database connection
    * @param sql SQL Statements
    * @return Error message in case if one of the requests is failed, an empty
    * string otherwise
    */
   static std::string execute(connection &conn, const char *sql);

private:
   //! Statement wrapper
   std::unique_ptr<detail::statement_ptr> stmt_;

   //! Database connection, this statement belongs to
   connection *connection_;

   //! Additional flags to the prepare command
   prepare_flags flags_;
};

} // namespace sqlite_burrito

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::statement::prepare_flags
operator|(sqlite_burrito::statement::prepare_flags lhs,
          sqlite_burrito::statement::prepare_flags rhs) {
   return static_cast<sqlite_burrito::statement::prepare_flags>(
       static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::statement::
    prepare_flags &
    operator|=(sqlite_burrito::statement::prepare_flags &lhs,
               sqlite_burrito::statement::prepare_flags rhs) {
   lhs = static_cast<sqlite_burrito::statement::prepare_flags>(
       static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
   return lhs;
}

#endif /* INCLUDE_SQLITE_BURRITO_STATEMENT_H */

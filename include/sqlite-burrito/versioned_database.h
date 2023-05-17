/**
 * @file   versioned_database.h
 * @author Dennis Sitelew
 * @date   May 17, 2023
 */

#ifndef INCLUDE_SQLITE_BURRITO_VERSIONED_DATABASE_H
#define INCLUDE_SQLITE_BURRITO_VERSIONED_DATABASE_H

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>
#include <sqlite-burrito/export.h>

#include <functional>
#include <string_view>

namespace sqlite_burrito {

//! A database, which is tracking its own version number in a dedicated table, and is able to update itself, using
//! the specified update callback
class SQLITE_BURRITO_EXPORT versioned_database {
public:
   //! Database update function type
   //! Performs a single update step
   //! The `from` parameter denotes the current database version
   using update_funct_t = std::function<void(versioned_database &db, int from, std::error_code &ec)>;

public:
   explicit versioned_database(connection::open_flags flags = connection::open_flags::default_mode);

   versioned_database(versioned_database &) = delete;
   versioned_database(versioned_database &&) = default;

   ~versioned_database();

public:
   versioned_database &operator=(versioned_database &) = delete;
   versioned_database &operator=(versioned_database &&) = default;

public:
   void open(std::string_view path,
             int current_version,
             const update_funct_t &update_func,
             std::string_view version_table = "metadata",
             std::string_view version_column = "version");

   void open(std::string_view path,
             int current_version,
             const update_funct_t &update_func,
             std::error_code &ec,
             std::string_view version_table = "metadata",
             std::string_view version_column = "version") noexcept;

public:
   [[nodiscard]] connection &get_connection() { return con_; }
   [[nodiscard]] const connection &get_connection() const { return con_; }

private:
   int get_file_version(std::string_view table, std::string_view column, std::error_code &ec);

   void perform_update(int from,
                       int to,
                       const update_funct_t &func,
                       std::string_view table,
                       std::string_view column,
                       std::error_code &ec);
   void store_new_version(int version, std::error_code &ec);

   void prepare_update_statement(std::string_view table, std::string_view column, std::error_code &ec);

private:
   //! Database connection
   connection con_;

   //! Database update statement
   statement update_stmt_;

   //! We cannot prepare the update statement, until the first update statement is executed (one which creates the
   //! metadata table) so we have to postpone it's construction and use this flag to indicate whether the update
   //! statement is ready or not.
   bool update_prepared_{false};
};

} // namespace sqlite_burrito

#endif // INCLUDE_SQLITE_BURRITO_VERSIONED_DATABASE_H

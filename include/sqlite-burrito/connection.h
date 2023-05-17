/**
 * @file   connection.h
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_CONNECTION_H
#define INCLUDE_SQLITE_BURRITO_CONNECTION_H

#include <sqlite-burrito/config.h>
#include <sqlite-burrito/export.h>

#include <sqlite3.h>

#include <memory>
#include <string>
#include <string_view>
#include <system_error>

namespace sqlite_burrito {

SQLITE_BURRITO_EXPORT class connection {
public:
   enum class open_flags : int {
      //! The database is opened in read-only mode. If the database does not
      //! already exist, an error is returned.
      readonly = SQLITE_OPEN_READONLY,

      //! The database is opened for reading and writing if possible, or reading
      //! only if the file is write protected by the operating system. In either
      //! case the database must already exist, otherwise an error is returned.
      readwrite = SQLITE_OPEN_READWRITE,

      create = SQLITE_OPEN_CREATE,

      //! The database is opened for reading and writing, and is created if it
      //! does not already exist.
      default_mode = create | readwrite,

      //! The filename can be interpreted as a URI if this flag is set.
      uri = SQLITE_OPEN_URI,

      //! The database will be opened as an in-memory database. The database is
      //! named by the "filename" argument for the purposes of cache-sharing, if
      //! shared cache mode is enabled, but the "filename" is otherwise ignored.
      memory = SQLITE_OPEN_MEMORY,

      //! The new database connection will use the "multi-thread" threading
      //! mode. This means that separate threads are allowed to use SQLite at
      //! the same time, as long as each thread is using a different database
      //! connection.
      no_mutex = SQLITE_OPEN_NOMUTEX,

      //! The new database connection will use the "serialized" threading mode.
      //! This means the multiple threads can safely attempt to use the same
      //! database connection at the same time. (Mutexes will block any actual
      //! concurrency, but in this mode there is no harm in trying.)
      full_mutex = SQLITE_OPEN_FULLMUTEX,

      //! The database is opened shared cache enabled, overriding the default
      //! shared cache setting provided by sqlite3_enable_shared_cache().
      shared_cache = SQLITE_OPEN_SHAREDCACHE,

      //! The database is opened shared cache disabled, overriding the default
      //! shared cache setting provided by sqlite3_enable_shared_cache().
      private_cache = SQLITE_OPEN_PRIVATECACHE,
   };

   using native_handle_t = ::sqlite3 *;

public:
   explicit connection(open_flags flags = open_flags::default_mode);

   connection(connection &) = delete;
   connection(connection &&) = default;

   ~connection();

public:
   connection &operator=(connection &) = delete;
   connection &operator=(connection &&) = default;

public:
   void open(std::string_view filename);
   void open(std::string_view filename, std::error_code &ec) noexcept;

public:
   [[nodiscard]] std::int64_t last_insert_rowid();


   [[nodiscard]] std::error_code last_error() const noexcept;

   [[nodiscard]] auto &native_handle() noexcept { return *connection_; };
   [[nodiscard]] const auto &native_handle() const noexcept { return *connection_; }

private:
   //! Database open flags
   open_flags flags_;

   //! Native handle
   native_handle_t connection_;
};

} // namespace sqlite_burrito

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::connection::open_flags operator|(
    sqlite_burrito::connection::open_flags lhs,
    sqlite_burrito::connection::open_flags rhs) {
   return static_cast<sqlite_burrito::connection::open_flags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

SQLITE_BURRITO_EXPORT constexpr inline sqlite_burrito::connection::open_flags &operator|=(
    sqlite_burrito::connection::open_flags &lhs,
    sqlite_burrito::connection::open_flags rhs) {
   lhs = static_cast<sqlite_burrito::connection::open_flags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
   return lhs;
}

#endif /* INCLUDE_SQLITE_BURRITO_CONNECTION_H */

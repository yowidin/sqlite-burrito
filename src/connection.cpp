/**
 * @file   connection.cpp
 * @author Dennis Sitelew
 * @date   Jul. 14, 2020
 */

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/errors/sqlite.h>

#include <sqlite3.h>

#include <chrono>
#include <thread>

using namespace sqlite_burrito;

connection::connection(open_flags flags)
   : flags_{flags}
   , connection_{nullptr} {
   // Nothing to do here
}

connection::~connection() {
   using namespace std::chrono_literals;
   using namespace std::chrono;

   const auto close_timeout = 5s;
   const auto sleep_duration = 50ms;

   auto start = steady_clock::now();

   auto rc = ::sqlite3_close(connection_);
   while (rc != SQLITE_OK) {
      if ((steady_clock::now() - start) > close_timeout) {
         break;
      }

      std::this_thread::sleep_for(sleep_duration);
      rc = ::sqlite3_close(connection_);
   }
}

void connection::open(std::string_view filename) {
   std::error_code ec;
   open(filename, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void connection::open(std::string_view filename, std::error_code &ec) noexcept {
   native_handle_t new_connection{nullptr};
   int result = ::sqlite3_open_v2(filename.data(), &new_connection, static_cast<int>(flags_), nullptr);

   ec = errors::make_error_code(result);

   if (!ec && new_connection) {
      ::sqlite3_close(connection_);
      connection_ = new_connection;
   }
}

std::int64_t connection::last_insert_rowid() {
   return ::sqlite3_last_insert_rowid(connection_);
}

std::error_code connection::last_error() const noexcept {
   return errors::make_error_code(sqlite3_extended_errcode(connection_));
}

transaction connection::begin_transaction(transaction::behavior behavior) {
   return transaction{*this, behavior};
}

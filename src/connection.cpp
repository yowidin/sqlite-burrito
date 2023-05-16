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
   int result = ::sqlite3_open_v2(filename.data(), &connection_, static_cast<int>(flags_), nullptr);
   ec = errors::make_error_code(result);
}

std::error_code connection::get_last_error() const noexcept {
   return errors::make_error_code(sqlite3_extended_errcode(connection_));
}

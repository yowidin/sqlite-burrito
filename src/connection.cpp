/**
 * @file   connection.cpp
 * @author Dennis Sitelew
 * @date   Jul. 14, 2020
 */

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/errors/sqlite.h>
#include <sqlite-burrito/detail/connection_ptr.h>

#include <sqlite3.h>

using namespace sqlite_burrito;

connection::connection(open_flags flags)
   : flags_{flags}
   , connection_{new detail::connection_ptr{}} {
   // Nothing to do here
}

connection::~connection() = default;

void connection::open(const char *filename) {
   std::error_code ec;
   open(filename, ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void connection::open(const char *filename, std::error_code &ec) noexcept {
   ::sqlite3 *tmp;
   int result = ::sqlite3_open_v2(filename, &tmp, static_cast<int>(flags_), nullptr);
   connection_->reset(tmp);
   ec = to_error_code(result);
}

std::error_code connection::get_last_error() const noexcept {
   return to_error_code(sqlite3_extended_errcode(connection_->get()));
}

std::error_code connection::to_error_code(int code) noexcept {
   return errors::sqlite3::make_error_code(
       static_cast<errors::sqlite3::error>(code));
}

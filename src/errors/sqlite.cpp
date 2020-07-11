/**
 * @file   sqlite.cpp
 * @author Dennis Sitelew
 * @date   Jul. 01, 2020
 */

#include <sqlite-burrito/errors/sqlite.h>

using namespace sqlite_burrito::errors::sqlite3;

#include <string>

namespace detail {

struct sqlite3_error_category : std::error_category {
   const char *name() const noexcept override { return "sqlite3-error"; }

   std::string message(int ev) const override { return sqlite3_errstr(ev); }

   std::error_condition default_error_condition(
       int ev) const noexcept override {
      // Excerpt from the SQLite3 docs:
      // Note that the primary result code is always a part of the extended
      // result code. Given a full 32-bit extended result code, the application
      // can always find the corresponding primary result code merely by
      // extracting the least significant 8 bits of the extended result code.
      auto primary_result_code = static_cast<unsigned>(ev) & 0xFFu;
      return make_error_condition(static_cast<condition>(primary_result_code));
   }
};

struct sqlite3_condition_category : std::error_category {
   const char *name() const noexcept override { return "sqlite3-condition"; }

   std::string message(int ev) const override { return sqlite3_errstr(ev); }
};

} // namespace detail

namespace sqlite_burrito {
namespace errors {
namespace sqlite3 {

const std::error_category &sqlite3_error_category() noexcept {
   static const detail::sqlite3_error_category value;
   return value;
}

const std::error_category &sqlite3_condition_category() noexcept {
   static const detail::sqlite3_condition_category value;
   return value;
}

} // namespace sqlite3
} // namespace errors
} // namespace sqlite_burrito

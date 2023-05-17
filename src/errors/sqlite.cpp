/**
 * @file   sqlite.cpp
 * @author Dennis Sitelew
 * @date   Jul. 01, 2020
 */

#include <sqlite-burrito/errors/sqlite.h>

using namespace sqlite_burrito::errors;

#include <sqlite3.h>
#include <string>

namespace {

struct error_category : std::error_category {
   [[nodiscard]] const char *name() const noexcept override { return "sqlite3-error"; }
   [[nodiscard]] std::string message(int ev) const override { return sqlite3_errstr(ev); }
   [[nodiscard]] std::error_condition default_error_condition(int ev) const noexcept override {
      // Excerpt from the SQLite3 docs:
      // Note that the primary result code is always a part of the extended
      // result code. Given a full 32-bit extended result code, the application
      // can always find the corresponding primary result code merely by
      // extracting the least significant 8 bits of the extended result code.
      auto primary_result_code = static_cast<unsigned>(ev) & 0xFFu;
      return make_error_condition(static_cast<condition>(primary_result_code));
   }
};

struct condition_category : std::error_category {
   [[nodiscard]] const char *name() const noexcept override { return "sqlite3-condition"; }
   [[nodiscard]] std::string message(int ev) const override { return sqlite3_errstr(ev); }
};

} // namespace

namespace sqlite_burrito::errors {

const std::error_category &sqlite3_error_category() noexcept {
   static const error_category value;
   return value;
}

const std::error_category &sqlite3_condition_category() noexcept {
   static const condition_category value;
   return value;
}

} // namespace sqlite_burrito::errors

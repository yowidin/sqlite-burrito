/**
 * @file   statement_ptr.h
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_DETAIL_STATEMENT_PTR_H
#define INCLUDE_SQLITE_BURRITO_DETAIL_STATEMENT_PTR_H

#include <sqlite3.h>
#include <memory>

namespace sqlite_burrito {
namespace detail {

struct statement_deleter {
   void operator()(::sqlite3_stmt *p) const {
      if (p != nullptr) {
         ::sqlite3_finalize(p);
      }
   }
};

class statement_ptr
   : public std::unique_ptr<::sqlite3_stmt, statement_deleter> {};

} // namespace detail
} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_DETAIL_STATEMENT_PTR_H */

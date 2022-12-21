/**
 * @file   mutex_ptr.h
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_DETAIL_MUTEX_PTR_H
#define INCLUDE_SQLITE_BURRITO_DETAIL_MUTEX_PTR_H

#include <sqlite-burrito/export.h>

#include <sqlite3.h>

#include <memory>

namespace sqlite_burrito {
namespace detail {

SQLITE_BURRITO_EXPORT struct mutex_deleter {
   void operator()(::sqlite3_mutex *p) const {
      if (p != nullptr) {
         ::sqlite3_mutex_free(p);
      }
   }
};

SQLITE_BURRITO_EXPORT class mutex_ptr
   : public std::unique_ptr<::sqlite3_mutex, mutex_deleter> {
};

} // namespace detail
} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_DETAIL_MUTEX_PTR_H */

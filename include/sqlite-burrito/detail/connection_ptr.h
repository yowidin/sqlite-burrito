/**
 * @file   connection_ptr.h
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_DETAIL_CONNECTION_PTR_H
#define INCLUDE_SQLITE_BURRITO_DETAIL_CONNECTION_PTR_H

#include <sqlite3.h>
#include <memory>

namespace sqlite_burrito {
namespace detail {

struct connection_deleter {
   void operator()(sqlite3 *p) const {
      if (p != nullptr) {
         ::sqlite3_close(p);
      }
   }
};

class connection_ptr : public std::unique_ptr<sqlite3, connection_deleter> {};

} // namespace detail
} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_DETAIL_CONNECTION_PTR_H */

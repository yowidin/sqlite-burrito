/**
 * @file   mutex.h
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_MUTEX_H
#define INCLUDE_SQLITE_BURRITO_MUTEX_H

#include <sqlite-burrito/export.h>

#include <sqlite3.h>

#include <memory>

namespace sqlite_burrito {

//! Encapsulates an SQLite3 mutex and provides std-like access to it
SQLITE_BURRITO_EXPORT class mutex {
public:
   using native_handle_t = ::sqlite3_mutex *;

public:
   explicit mutex(bool recursive = false);
   ~mutex();

   mutex(mutex &) = delete;
   mutex(mutex &&) = default;

public:
   mutex &operator=(mutex &) = delete;
   mutex &operator=(mutex &&) = default;

public:
   // BasicLockable
   void lock();
   void unlock();

   // Lockable
   bool try_lock();

private:
   //! Native handle
   native_handle_t mutex_;
};

} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_MUTEX_H */

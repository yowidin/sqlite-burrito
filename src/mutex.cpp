/**
 * @file   mutex.cpp
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */

#include <sqlite-burrito/mutex.h>
#include <sqlite-burrito/detail/mutex_ptr.h>

#include <sqlite3.h>

using namespace sqlite_burrito;

mutex::mutex(bool recursive)
   : mutex_{new detail::mutex_ptr} {
   auto res = sqlite3_mutex_alloc(recursive ? isolation::sqlite_mutex_recursive
                                            : isolation::sqlite_mutex_fast);
   mutex_->reset(res);
}

mutex::~mutex() = default;

void mutex::lock() {
   sqlite3_mutex_enter(mutex_->get());
}

void mutex::unlock() {
   sqlite3_mutex_leave(mutex_->get());
}

bool mutex::try_lock() {
   return sqlite3_mutex_try(mutex_->get()) == isolation::sqlite_ok;
}

/**
 * @file   mutex.cpp
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */

#include <sqlite-burrito/mutex.h>

#include <sqlite3.h>

using namespace sqlite_burrito;

mutex::mutex(bool recursive)
   : mutex_{nullptr} {
   mutex_ = sqlite3_mutex_alloc(recursive ? SQLITE_MUTEX_RECURSIVE : SQLITE_MUTEX_FAST);
}

mutex::~mutex() = default;

void mutex::lock() {
   sqlite3_mutex_enter(mutex_);
}

void mutex::unlock() {
   sqlite3_mutex_leave(mutex_);
}

bool mutex::try_lock() {
   return sqlite3_mutex_try(mutex_) == SQLITE_OK;
}

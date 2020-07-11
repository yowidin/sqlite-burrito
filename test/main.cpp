/**
 * @file   main.cpp
 * @author Dennis Sitelew
 * @date   Jun. 25, 2020
 */

#include <sqlite-burrito/library.h>
#include <sqlite-burrito/errors/sqlite.h>

#include <iostream>

int main() {
   auto test = sqlite_burrito::errors::sqlite3::make_error_code(sqlite_burrito::errors::sqlite3::error::auth_user);

   std::cout << test.message() << std::endl;
   std::cout << (test == sqlite_burrito::errors::sqlite3::condition::ok) << std::endl;

   hello();
}

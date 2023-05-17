/**
 * @file   test_package.cpp
 * @author Dennis Sitelew
 * @date   May 17, 2023
 */

#include <cstdlib>
#include <sqlite-burrito/connection.h>

int main(void) {
   sqlite_burrito::connection con{};
   con.open(":memory:");
   return EXIT_SUCCESS;
}
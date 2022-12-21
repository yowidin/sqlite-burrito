/**
 * @file   connection.cpp
 * @author Dennis Sitelew
 * @date   Jul. 14, 2020
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/errors/sqlite.h>

using namespace sqlite_burrito;

TEST_CASE("Open flags should be combinable", "[connection][open_flags]") {
   connection::open_flags flags;
   flags = connection::open_flags::create | connection::open_flags::readwrite;
   flags |= connection::open_flags::full_mutex;
   REQUIRE(flags ==
           (connection::open_flags::create | connection::open_flags::readwrite |
            connection::open_flags::full_mutex));
   REQUIRE(static_cast<int>(flags) ==
           (static_cast<int>(connection::open_flags::create) |
            static_cast<int>(connection::open_flags::readwrite) |
            static_cast<int>(connection::open_flags::full_mutex)));
}

TEST_CASE("Opening in-memory connection should not fail", "[connection][open_flags]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));
}

TEST_CASE("Opening non-existent file should fail", "[connection][open_flags]") {
   const char *invalid_file_name = "./sqlite-burrito-connection-test.db";
   connection conn{connection::open_flags::readonly};
   REQUIRE_THROWS_AS(conn.open(invalid_file_name), std::runtime_error);

   std::error_code ec;
   REQUIRE_NOTHROW(conn.open(invalid_file_name, ec));
   REQUIRE(ec == errors::sqlite3::condition::cantopen);
}


// TODO: Test exec functions - simple and with callbacks
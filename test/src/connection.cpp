/**
 * @file   connection.cpp
 * @author Dennis Sitelew
 * @date   Jul. 14, 2020
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/errors/sqlite.h>

using namespace sqlite_burrito;

using flags_t = connection::open_flags;

TEST_CASE("Open flags should be combinable", "[connection][open_flags]") {
   flags_t flags;
   flags = flags_t::create | flags_t::readwrite;
   flags |= flags_t::full_mutex;
   REQUIRE(flags == (flags_t::create | flags_t::readwrite | flags_t::full_mutex));
   REQUIRE(static_cast<int>(flags) == (static_cast<int>(flags_t::create) | static_cast<int>(flags_t::readwrite) |
                                       static_cast<int>(flags_t::full_mutex)));
}

TEST_CASE("Opening in-memory connection using URI should not fail", "[connection][open_flags]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));
}

TEST_CASE("Opening in-memory connection using flags should not fail", "[connection][open_flags]") {
   connection conn{flags_t::memory | flags_t::default_mode};
   REQUIRE_NOTHROW(conn.open("test"));
}

TEST_CASE("Opening non-existent file should fail", "[connection][open_flags]") {
   const char *invalid_file_name = "./sqlite-burrito-connection-test.db";
   connection conn{flags_t::readonly};
   REQUIRE_THROWS_AS(conn.open(invalid_file_name), std::runtime_error);

   std::error_code ec;
   REQUIRE_NOTHROW(conn.open(invalid_file_name, ec));
   REQUIRE(ec == errors::condition::cantopen);
}
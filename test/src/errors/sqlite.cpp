/**
 * @file   sqlite.cpp
 * @author Dennis Sitelew
 * @date   Jul. 12, 2020
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/errors/sqlite.h>

using namespace sqlite_burrito::errors;

TEST_CASE("OK code should not count as error", "[errors]") {
   std::error_code ec = error::ok;
   REQUIRE(!ec);
   REQUIRE(ec == condition::ok);
}

TEST_CASE("Error codes should map to error conditions", "[errors]") {
   std::error_code ec = error::ioerr_read;
   REQUIRE(ec);
   REQUIRE(ec == condition::ioerr);

   ec = error::ioerr_blocked;
   REQUIRE(ec);
   REQUIRE(ec == condition::ioerr);
}

TEST_CASE("Error codes should provide a message", "[errors]") {
   std::error_code ec = error::ioerr_read;
   REQUIRE(ec);
   REQUIRE(!ec.message().empty());

   REQUIRE(!std::string(ec.category().name()).empty());

   ec = error::ioerr_blocked;
   REQUIRE(ec);
   REQUIRE(!ec.message().empty());
}

TEST_CASE("Error conditions should provide a message", "[errors]") {
   std::error_condition ec = condition::ioerr;
   REQUIRE(ec);
   REQUIRE(!ec.message().empty());

   REQUIRE(!std::string(ec.category().name()).empty());

   ec = condition::auth;
   REQUIRE(ec);
   REQUIRE(!ec.message().empty());
}

TEST_CASE("Test make functions", "[errors]") {
   auto err = make_error_code(error::ioerr_read);
   REQUIRE(err);
   REQUIRE(err.category() == sqlite3_error_category());

   auto cond = make_error_condition(condition::ioerr);
   REQUIRE(cond);
   REQUIRE(cond.category() == sqlite3_condition_category());

   REQUIRE(err == cond);
}

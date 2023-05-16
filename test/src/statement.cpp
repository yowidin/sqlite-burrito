/**
 * @file   statement.cpp
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>

#include <iterator>

using namespace sqlite_burrito;

TEST_CASE("Prepare should return a valid iterator", "[statement][prepare]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));

   statement stmt{conn};

   SECTION("in the middle on success") {
      std::string_view request{"CREATE TABLE test(foo INT);INSERT INTO test VALUES(10);"};
      std::error_code ec;
      auto it = stmt.prepare(request, ec);

      REQUIRE(!ec);

      auto offset = std::distance(std::cbegin(request), it);
      auto idx = request.find(';');

      REQUIRE(offset == (idx + 1));
   }

   SECTION("at the end on success") {
      std::string_view request = "CREATE TABLE test(foo INT);";
      std::error_code ec;
      auto it = stmt.prepare(request, ec);

      REQUIRE(!ec);
      REQUIRE(it == std::end(request));
   }
}

TEST_CASE("Invalid statements should return an error", "[statement][prepare]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));

   statement stmt{conn};
   std::string request = "FOO BAR;";
   REQUIRE_THROWS_AS(stmt.prepare(request), std::runtime_error);
}

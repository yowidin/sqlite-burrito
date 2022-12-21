/**
 * @file   bind_parameters.cpp
 * @author Dennis Sitelew
 * @date   Jul. 23, 2020
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>
#include <sqlite-burrito/bind_parameters.h>
#include <sqlite-burrito/errors/sqlite.h>
#include <iostream>

#include <iterator>

using namespace sqlite_burrito;

TEST_CASE("Basic iterator operations on bind parameters", "[bind_parameters]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));

   std::error_code ec;
   auto err = statement::execute(conn, "CREATE TABLE test(id INT, name TEXT);", ec);
   REQUIRE(!ec);

   statement stmt{conn};
   REQUIRE_NOTHROW(stmt.prepare("INSERT INTO test(id, name) VALUES(:id, :name)"));

   bind_parameters params{stmt};
   REQUIRE(*params.begin() == ":id");
   REQUIRE(params.begin()[1] == ":name");
}

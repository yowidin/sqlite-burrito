/**
 * @file   versioned_database.cpp
 * @author Dennis Sitelew
 * @date   May 17, 2023
 */

#include <catch2/catch_test_macros.hpp>

#include <sqlite-burrito/versioned_database.h>

#include <system_error>

using namespace sqlite_burrito;

namespace {

const auto max_version = 3;

void update_fn(versioned_database &db, int from, std::error_code &ec) {
   auto v0 = R"sql(
BEGIN TRANSACTION;

CREATE TABLE metadata(version INTEGER);

INSERT INTO metadata(version)
VALUES (0);

COMMIT;
)sql";

   auto v1 = R"sql(
BEGIN TRANSACTION;

CREATE TABLE test(value INTEGER);

COMMIT;
)sql";

   auto v2 = R"sql(
BEGIN TRANSACTION;

INSERT INTO test(value)
VALUES (0), (1), (2), (3);

COMMIT;
)sql";

   switch (from) {
      case 0:
         statement::execute(db.get_connection(), v0, ec);
         break;

      case 1:
         statement::execute(db.get_connection(), v1, ec);
         break;

      case 2:
         statement::execute(db.get_connection(), v2, ec);
         break;

      default:
         ec = std::make_error_code(std::errc::invalid_argument);
         return;
   }
}

} // namespace

TEST_CASE("Errors should be propagated", "[versioned_database]") {
   versioned_database db;
   REQUIRE_THROWS_AS(db.open(":memory:", 4, update_fn), std::system_error);
}

TEST_CASE("The latest database version should be stored", "[versioned_database]") {
   versioned_database db;
   REQUIRE_NOTHROW(db.open(":memory:", max_version, update_fn));

   statement get_version{db.get_connection()};
   REQUIRE_NOTHROW(get_version.prepare("SELECT version FROM metadata;"));
   REQUIRE_NOTHROW(get_version.step());

   int version;
   REQUIRE_NOTHROW(get_version.get(0, version));
   REQUIRE(version == max_version);
}

TEST_CASE("Values should be accessible", "[versioned_database]") {
   versioned_database db;
   REQUIRE_NOTHROW(db.open(":memory:", max_version, update_fn));

   statement get_values{db.get_connection()};
   REQUIRE_NOTHROW(get_values.prepare("SELECT value FROM test;"));

   std::vector<int> values;
   while (get_values.step()) {
      int tmp;
      REQUIRE_NOTHROW(get_values.get(0, tmp));
      values.push_back(tmp);
   }
}

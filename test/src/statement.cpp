/**
 * @file   statement.cpp
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */

#include <catch2/catch_test_macros.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>

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

TEST_CASE("Bindings should not be ambiguous", "[statement][bind]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));

   REQUIRE_NOTHROW(statement::execute(conn,
                                      R"sql(;
CREATE TABLE IF NOT EXISTS test(
   int_value INTEGER,
   double_value DOUBLE,
   text_value TEXT,
   blob_value BLOB
);
)sql"));

   statement stmt{conn};
   REQUIRE_NOTHROW(
       stmt.prepare(R"sql(INSERT INTO test(int_value, double_value, text_value, blob_value) VALUES(?, ?, ?, ?);)sql"));

   REQUIRE_NOTHROW(stmt.execute());

   // Binding without a reset should throw
   REQUIRE_THROWS_AS(stmt.bind(1, 10), std::runtime_error);

   REQUIRE_NOTHROW(stmt.reset());

   SECTION("int types") {
      std::int8_t i8 = 1;
      std::int16_t i16 = 2;
      std::int32_t i32 = 3;
      std::int64_t i64 = 4;

      REQUIRE_NOTHROW(stmt.bind(1, i8));
      REQUIRE_NOTHROW(stmt.bind(2, i16));
      REQUIRE_NOTHROW(stmt.bind(3, i32));
      REQUIRE_NOTHROW(stmt.bind(4, i64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int types") {
      const std::int8_t i8 = 1;
      const std::int16_t i16 = 2;
      const std::int32_t i32 = 3;
      const std::int64_t i64 = 4;

      REQUIRE_NOTHROW(stmt.bind(1, i8));
      REQUIRE_NOTHROW(stmt.bind(2, i16));
      REQUIRE_NOTHROW(stmt.bind(3, i32));
      REQUIRE_NOTHROW(stmt.bind(4, i64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("uint types") {
      std::uint8_t u8 = 1;
      std::uint16_t u16 = 2;
      std::uint32_t u32 = 3;
      std::uint64_t u64 = 4;

      REQUIRE_NOTHROW(stmt.bind(1, u8));
      REQUIRE_NOTHROW(stmt.bind(2, u16));
      REQUIRE_NOTHROW(stmt.bind(3, u32));
      REQUIRE_NOTHROW(stmt.bind(4, u64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const uint types") {
      std::uint8_t u8 = 1;
      std::uint16_t u16 = 2;
      std::uint32_t u32 = 3;
      std::uint64_t u64 = 4;

      REQUIRE_NOTHROW(stmt.bind(1, u8));
      REQUIRE_NOTHROW(stmt.bind(2, u16));
      REQUIRE_NOTHROW(stmt.bind(3, u32));
      REQUIRE_NOTHROW(stmt.bind(4, u64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("null type") {
      REQUIRE_NOTHROW(stmt.bind_null(1));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("bool type") {
      REQUIRE_NOTHROW(stmt.bind(1, true));
      REQUIRE_NOTHROW(stmt.bind(2, false));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("real types") {
      float f = 3.14f;
      double d = 3.14;

      REQUIRE_NOTHROW(stmt.bind(1, f));
      REQUIRE_NOTHROW(stmt.bind(1, d));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const real types") {
      const float f = 3.14f;
      const double d = 3.14;

      REQUIRE_NOTHROW(stmt.bind(1, f));
      REQUIRE_NOTHROW(stmt.bind(2, d));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("char array") {
      char data[32] = {0};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const char array") {
      const char data[32] = {0};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int array") {
      int data[32] = {42};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int std::array") {
      std::array<int, 2> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int std::array") {
      const std::array<int, 2> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int vector") {
      std::vector<int> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int vector") {
      const std::vector<int> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("optional int vector") {
      std::optional<std::vector<int>> data = {std::vector<int>({10, 20, 30})};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("optional int vector") {
      std::optional<std::vector<int>> data = {};
      REQUIRE_NOTHROW(stmt.bind(4, data));
      REQUIRE_NOTHROW(stmt.execute());
   }
}

TEST_CASE("Name-based bindings should not be ambiguous", "[statement][bind]") {
   connection conn;
   REQUIRE_NOTHROW(conn.open(":memory:"));

   REQUIRE_NOTHROW(statement::execute(conn,
                                      R"sql(;
CREATE TABLE IF NOT EXISTS test(
   int_value INTEGER,
   double_value DOUBLE,
   text_value TEXT,
   blob_value BLOB
);
)sql"));

   statement stmt{conn};
   REQUIRE_NOTHROW(stmt.prepare(
       R"sql(INSERT INTO test(int_value, double_value, text_value, blob_value)
             VALUES(:pint, :pdouble, :ptext, :pblob);)sql"));

   REQUIRE_NOTHROW(stmt.execute());

   // Binding without a reset should throw
   REQUIRE_THROWS_AS(stmt.bind(1, 10), std::runtime_error);

   REQUIRE_NOTHROW(stmt.reset());

   SECTION("int types") {
      std::int8_t i8 = 1;
      std::int16_t i16 = 2;
      std::int32_t i32 = 3;
      std::int64_t i64 = 4;

      REQUIRE_NOTHROW(stmt.bind(":pint", i8));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", i16));
      REQUIRE_NOTHROW(stmt.bind(":ptext", i32));
      REQUIRE_NOTHROW(stmt.bind(":pblob", i64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int types") {
      const std::int8_t i8 = 1;
      const std::int16_t i16 = 2;
      const std::int32_t i32 = 3;
      const std::int64_t i64 = 4;

      REQUIRE_NOTHROW(stmt.bind(":pint", i8));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", i16));
      REQUIRE_NOTHROW(stmt.bind(":ptext", i32));
      REQUIRE_NOTHROW(stmt.bind(":pblob", i64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("uint types") {
      std::uint8_t u8 = 1;
      std::uint16_t u16 = 2;
      std::uint32_t u32 = 3;
      std::uint64_t u64 = 4;

      REQUIRE_NOTHROW(stmt.bind(":pint", u8));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", u16));
      REQUIRE_NOTHROW(stmt.bind(":ptext", u32));
      REQUIRE_NOTHROW(stmt.bind(":pblob", u64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const uint types") {
      std::uint8_t u8 = 1;
      std::uint16_t u16 = 2;
      std::uint32_t u32 = 3;
      std::uint64_t u64 = 4;

      REQUIRE_NOTHROW(stmt.bind(":pint", u8));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", u16));
      REQUIRE_NOTHROW(stmt.bind(":ptext", u32));
      REQUIRE_NOTHROW(stmt.bind(":pblob", u64));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("null type") {
      REQUIRE_NOTHROW(stmt.bind_null(":pint"));
      REQUIRE_NOTHROW(stmt.bind_null(":pdouble"));
      REQUIRE_NOTHROW(stmt.bind_null(":ptext"));
      REQUIRE_NOTHROW(stmt.bind_null(":pblob"));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("real types") {
      float f = 3.14f;
      double d = 3.14;

      REQUIRE_NOTHROW(stmt.bind(":pint", f));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", d));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const real types") {
      const float f = 3.14f;
      const double d = 3.14;

      REQUIRE_NOTHROW(stmt.bind(":pint", f));
      REQUIRE_NOTHROW(stmt.bind(":pdouble", d));

      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("char array") {
      char data[32] = {0};
      REQUIRE_NOTHROW(stmt.bind(":ptext", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const char array") {
      const char data[32] = {0};
      REQUIRE_NOTHROW(stmt.bind(":ptext", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int array") {
      int data[32] = {42};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int std::array") {
      std::array<int, 2> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int std::array") {
      const std::array<int, 2> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("int vector") {
      std::vector<int> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("const int vector") {
      const std::vector<int> data = {42, 32};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("optional int vector") {
      std::optional<std::vector<int>> data = {std::vector<int>({10, 20, 30})};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }

   SECTION("optional int vector") {
      std::optional<std::vector<int>> data = {};
      REQUIRE_NOTHROW(stmt.bind(":pblob", data));
      REQUIRE_NOTHROW(stmt.execute());
   }
}

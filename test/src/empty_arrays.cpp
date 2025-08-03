/**
 * @file   empty_arrays.cpp
 * @author Dennis Sitelew
 * @date   May 26, 2023
 */

#include <catch2/catch_test_macros.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>

#include <system_error>

using namespace sqlite_burrito;

namespace {

class empty_array_test {
public:
   struct entry {
      std::string txt;
      std::vector<std::uint8_t> bin;
   };

public:
   empty_array_test() {
      con_.open(":memory:");
      statement::execute(con_, "CREATE TABLE test(txt TEXT, bin BLOB);");
      select_.prepare("SELECT txt, bin FROM test WHERE rowid == :prowid;");
      insert_.prepare("INSERT INTO test(txt, bin) VALUES (:ptxt, :pbin);");
   }

public:
   void add_entry(const entry &e) {
      insert_.reset();
      insert_.bind(":ptxt", e.txt);
      insert_.bind(":pbin", e.bin);
      insert_.execute();
   }

   entry get_entry(std::int64_t rowid) {
      select_.reset();
      select_.bind(":prowid", rowid);
      select_.step();

      entry result{};
      select_.get(0, result.txt);
      select_.get(1, result.bin);
      return result;
   }

   std::uint64_t last_rowid() { return con_.last_insert_rowid(); }

protected:
   connection con_{};
   statement select_{con_};
   statement insert_{con_};
};

} // namespace

TEST_CASE_METHOD(empty_array_test, "Getting empty arrays should not fail", "[statement][get]") {
   REQUIRE_NOTHROW(add_entry({"a", std::vector<std::uint8_t>{1}}));
   auto txt_bin = last_rowid();

   REQUIRE_NOTHROW(add_entry({"", std::vector<std::uint8_t>{1}}));
   auto empty_bin = last_rowid();

   REQUIRE_NOTHROW(add_entry({"a", std::vector<std::uint8_t>{}}));
   auto txt_empty = last_rowid();

   REQUIRE_NOTHROW(add_entry({"", std::vector<std::uint8_t>{}}));
   auto empty_empty = last_rowid();

   SECTION("Getting string and a vector should not fail") {
      entry e;
      REQUIRE_NOTHROW(e = get_entry(txt_bin));
      REQUIRE(!e.txt.empty());
      REQUIRE(!e.bin.empty());
   }

   SECTION("Getting EMPTY string and a vector should not fail") {
      entry e;
      REQUIRE_NOTHROW(e = get_entry(empty_bin));
      REQUIRE(e.txt.empty());
      REQUIRE(!e.bin.empty());
   }

   SECTION("Getting a string and an EMPTY vector should not fail") {
      entry e;
      REQUIRE_NOTHROW(e = get_entry(txt_empty));
      REQUIRE(!e.txt.empty());
      REQUIRE(e.bin.empty());
   }

   SECTION("Getting an EMPTY string and an EMPTY vector should not fail") {
      entry e;
      REQUIRE_NOTHROW(e = get_entry(empty_empty));
      REQUIRE(e.txt.empty());
      REQUIRE(e.bin.empty());
   }
}

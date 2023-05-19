/**
 * @file   transaction.cpp
 * @author Dennis Sitelew
 * @date   May 19, 2023
 */

#include <catch2/catch.hpp>

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>
#include <sqlite-burrito/transaction.h>

#include <system_error>

using namespace sqlite_burrito;

namespace {

class transaction_test {
public:
   transaction_test() {
      con_.open(":memory:");
      statement::execute(con_, "CREATE TABLE test(value TEXT);");
      select_.prepare("SELECT COUNT(*) FROM test;");
      insert_.prepare("INSERT INTO test(value) VALUES (:pvalue);");
   }

public:
   int count_entries() {
      int result;
      select_.reset();
      select_.step();
      select_.get(0, result);
      return result;
   }

   void add_entry(const char *entry) {
      insert_.reset();
      insert_.bind(":pvalue", entry);
      insert_.execute();
   }

protected:
   connection con_{};
   statement select_{con_};
   statement insert_{con_};
};

} // namespace

TEST_CASE_METHOD(transaction_test, "Commit should propagate changes", "[transaction]") {
   REQUIRE(count_entries() == 0);

   auto trans = con_.begin_transaction();
   REQUIRE_NOTHROW(add_entry("test"));
   REQUIRE_NOTHROW(trans.commit());
   REQUIRE(count_entries() == 1);

   // Already committed
   REQUIRE_THROWS_AS(trans.commit(), std::system_error);
   REQUIRE_THROWS_AS(trans.rollback(), std::system_error);
}

TEST_CASE_METHOD(transaction_test, "Rollback should discard changes", "[transaction]") {
   REQUIRE(count_entries() == 0);

   transaction trans{con_};
   REQUIRE_NOTHROW(add_entry("test"));
   REQUIRE_NOTHROW(trans.rollback());
   REQUIRE(count_entries() == 0);

   // Already committed
   REQUIRE_THROWS_AS(trans.commit(), std::system_error);
   REQUIRE_THROWS_AS(trans.rollback(), std::system_error);
}

TEST_CASE_METHOD(transaction_test, "Auto-rollback on scope exit", "[transaction]") {
   REQUIRE(count_entries() == 0);
   {
      transaction trans{con_};
      REQUIRE_NOTHROW(add_entry("test"));
   }
   REQUIRE(count_entries() == 0);
}

TEST_CASE_METHOD(transaction_test, "Manual-rollback should not result in errors on scope exit", "[transaction]") {
   REQUIRE(count_entries() == 0);
   {
      transaction trans{con_};
      REQUIRE_NOTHROW(add_entry("test"));
      trans.rollback();
   }
   REQUIRE(count_entries() == 0);
}

TEST_CASE_METHOD(transaction_test, "Manual-commit should not result in errors on scope exit", "[transaction]") {
   REQUIRE(count_entries() == 0);
   {
      transaction trans{con_};
      REQUIRE_NOTHROW(add_entry("test"));
      trans.commit();
   }
   REQUIRE(count_entries() == 1);
}

TEST_CASE_METHOD(transaction_test, "Changes after rolling back should be propagated", "[transaction]") {
   REQUIRE(count_entries() == 0);

   {
      transaction trans{con_};
      REQUIRE_NOTHROW(add_entry("1"));
      trans.rollback();

      REQUIRE_NOTHROW(add_entry("2"));
      REQUIRE_NOTHROW(add_entry("3"));
   }

   REQUIRE(count_entries() == 2);
}

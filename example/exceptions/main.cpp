/**
 * @file   main.cpp
 * @author Dennis Sitelew
 * @date   May 21, 2023
 */

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/statement.h>
#include <sqlite-burrito/versioned_database.h>

#include <iostream>
#include <optional>
#include <string>

using namespace sqlite_burrito;

// Abstraction over a single database table
class dummy_table {
public:
   // Dummy entry type
   struct entry {
      int first{};
      std::optional<std::string> second{};
      std::string third{};
   };

public:
   explicit dummy_table(connection &con)
      : add_entry_{con, statement::prepare_flags::persistent}
      , select_all_{con, statement::prepare_flags::persistent}
      , by_rowid_{con, statement::prepare_flags::persistent} {
      // Nothing to do here
   }

public:
   void prepare_statements() {
      add_entry_.prepare(R"sql(INSERT INTO test(first, second, third) VALUES(:pfirst, :psecond, :pthird);)sql");
      select_all_.prepare(R"sql(SELECT first, second, third FROM test;)sql");
      by_rowid_.prepare(R"sql(SELECT first, second, third FROM test WHERE rowid == :prowid;)sql");
   }

   void add_entry(const entry &e) {
      add_entry_.reset();
      add_entry_.bind(":pfirst", e.first);
      add_entry_.bind(":psecond", e.second);
      add_entry_.bind(3, e.third);
      add_entry_.execute();
   }

   void select_all(std::vector<entry> &v) {
      v.clear();

      select_all_.reset();
      while (select_all_.step()) {
         auto &e = v.emplace_back();
         select_all_.get(0, e.first);
         select_all_.get(1, e.second);
         select_all_.get(2, e.third);
      }
   }

   std::optional<entry> by_row_id(std::int64_t row_id) {
      by_rowid_.reset();
      by_rowid_.bind(":prowid", row_id);
      if (!by_rowid_.step()) {
         return std::nullopt;
      }

      entry e{};
      by_rowid_.get(0, e.first);
      by_rowid_.get(1, e.second);
      by_rowid_.get(2, e.third);
      return e;
   }

private:
   statement add_entry_;
   statement select_all_;
   statement by_rowid_;
};

// Dummy database class, capable of updating itself
class dummy_database : public versioned_database {
public:
   static constexpr int DATABASE_VERSION = 3;

public:
   explicit dummy_database(connection::open_flags flags = connection::open_flags::default_mode)
      : versioned_database{flags}
      , table_{get_connection()} {
      // Nothing to do here
   }

public:
   void open(std::string_view path) {
      versioned_database::open(path, DATABASE_VERSION, do_update);
      table_.prepare_statements();
   }

   dummy_table &dummy() { return table_; };

private:
   // Function to be called when performing a database update
   static void do_update(versioned_database &db, int from, std::error_code &ec) {
      const char *sql;
      switch (from) {
         case 0:
            sql = R"sql(BEGIN TRANSACTION;
                        CREATE TABLE metadata(version INTEGER);
                        INSERT INTO  metadata(version) VALUES(0);
                        COMMIT;)sql";
            break;

         case 1:
            sql = "CREATE TABLE test(first INTEGER NOT NULL, second TEXT, third TEXT NOT NULL);";
            break;

         case 2:
            sql = R"sql(INSERT INTO test(first, second, third) VALUES (1, NULL, "1"), (2, "II", "2");)sql";
            break;

         default:
            // Bad version
            ec = std::make_error_code(std::errc::invalid_argument);
            return;
      }

      auto err = statement::execute(db.get_connection(), sql, ec);
      if (err) {
         std::cerr << "Update error: " << *err << std::endl;
      }
   }

private:
   dummy_table table_;
};

// Print a database entry
std::ostream &operator<<(std::ostream &os, const dummy_table::entry &e) {
   os << "entry(";
   os << e.first;
   os << ", ";
   if (e.second) {
      os << '"' << *e.second << '"';
   } else {
      os << "[NULL]";
   }
   os << ", ";
   os << '"' << e.third << "\")";
   return os;
}

int main_checked() {
   dummy_database db;
   db.open(":memory:");

   std::vector<dummy_table::entry> entries;
   auto &dummy = db.dummy();

   dummy.select_all(entries);
   std::cout << "Existing entries:" << std::endl;
   for (const auto &e : entries) {
      std::cout << e << std::endl;
   }

   dummy.add_entry({3, "III", "three"});

   auto row_id = db.get_connection().last_insert_rowid();
   std::cout << "Inserted entry ID: " << row_id << std::endl;

   auto maybe_entry = dummy.by_row_id(row_id);
   if (!maybe_entry) {
      std::cerr << "Could not find inserted entry" << std::endl;
      return -1;
   }

   std::cout << "Inserted entry: " << *maybe_entry << std::endl;
   return 0;
}

int main(int, char **) {
   try {
      return main_checked();
   } catch (std::exception &e) {
      std::cerr << "std::exception: " << e.what() << std::endl;
      return -1;
   } catch (...) {
      std::cerr << "unexpected exception" << std::endl;
      return -1;
   }
}
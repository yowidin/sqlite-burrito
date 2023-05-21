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

// Helper macro to return from a function in case if a std::error_code named `ec` indicates an error
#define RETURN_ON_EC(expr) \
   do {                    \
      (expr);              \
      if ((ec)) {          \
         return;           \
      }                    \
   } while (false)

// Same as above, but return an empty optional in case of an error
#define RETURN_NULL_OPT_ON_EC(expr) \
   do {                             \
      (expr);                       \
      if ((ec)) {                   \
         return std::nullopt;       \
      }                             \
   } while (false)

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
   void prepare_statements(std::error_code &ec) {
      RETURN_ON_EC(add_entry_.prepare(
          R"sql(INSERT INTO test(first, second, third) VALUES(:pfirst, :psecond, :pthird);)sql", ec));
      RETURN_ON_EC(select_all_.prepare(R"sql(SELECT first, second, third FROM test;)sql", ec));
      RETURN_ON_EC(by_rowid_.prepare(R"sql(SELECT first, second, third FROM test WHERE rowid == :prowid;)sql", ec));
   }

   void add_entry(const entry &e, std::error_code &ec) {
      RETURN_ON_EC(add_entry_.reset(ec));
      RETURN_ON_EC(add_entry_.bind(":pfirst", e.first, ec));
      RETURN_ON_EC(add_entry_.bind(":psecond", e.second, ec));
      RETURN_ON_EC(add_entry_.bind(3, e.third, ec));
      RETURN_ON_EC(add_entry_.execute(ec));
   }

   void select_all(std::vector<entry> &v, std::error_code &ec) {
      v.clear();

      RETURN_ON_EC(select_all_.reset(ec));
      while (select_all_.step(ec) && !ec) {
         auto &e = v.emplace_back();
         RETURN_ON_EC(select_all_.get(0, e.first, ec));
         RETURN_ON_EC(select_all_.get(1, e.second, ec));
         RETURN_ON_EC(select_all_.get(2, e.third, ec));
      }
   }

   std::optional<entry> by_row_id(std::int64_t row_id, std::error_code &ec) {
      RETURN_NULL_OPT_ON_EC(by_rowid_.reset(ec));
      RETURN_NULL_OPT_ON_EC(by_rowid_.bind(":prowid", row_id, ec));
      if (!by_rowid_.step(ec) || ec) {
         return std::nullopt;
      }

      entry e{};
      RETURN_NULL_OPT_ON_EC(by_rowid_.get(0, e.first, ec));
      RETURN_NULL_OPT_ON_EC(by_rowid_.get(1, e.second, ec));
      RETURN_NULL_OPT_ON_EC(by_rowid_.get(2, e.third, ec));
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
   void open(std::string_view path, std::error_code &ec) {
      RETURN_ON_EC(versioned_database::open(path, DATABASE_VERSION, do_update, ec));
      table_.prepare_statements(ec);
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

int main(int, char **) {
   std::error_code ec;

   dummy_database db;
   db.open(":memory:", ec);
   if (ec) {
      std::cerr << "Open error: " << ec.message() << std::endl;
      return -1;
   }

   std::vector<dummy_table::entry> entries;
   auto &dummy = db.dummy();

   dummy.select_all(entries, ec);
   if (ec) {
      std::cerr << "Select all error: " << ec.message() << std::endl;
      return -1;
   }

   std::cout << "Existing entries:" << std::endl;
   for (const auto &e : entries) {
      std::cout << e << std::endl;
   }

   dummy.add_entry({3, "III", "three"}, ec);
   if (ec) {
      std::cerr << "Error adding an entry: " << ec.message() << std::endl;
      return -1;
   }

   auto row_id = db.get_connection().last_insert_rowid();
   std::cout << "Inserted entry ID: " << row_id << std::endl;

   auto maybe_entry = dummy.by_row_id(row_id, ec);
   if (ec) {
      std::cerr << "Could not find inserted entry: " << ec.message() << std::endl;
      return -1;
   }

   if (!maybe_entry) {
      std::cerr << "Could not find inserted entry" << std::endl;
      return -1;
   }
   std::cout << "Inserted entry: " << *maybe_entry << std::endl;

   return 0;
}
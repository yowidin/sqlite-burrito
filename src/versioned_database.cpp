/**
 * @file   versioned_database.cpp
 * @author Dennis Sitelew
 * @date   May 17, 2023
 */

#include <sqlite-burrito/versioned_database.h>

#include <sstream>

using namespace sqlite_burrito;

versioned_database::versioned_database(connection::open_flags flags)
   : con_{flags}
   , update_stmt_{con_} {
   // Nothing to do here
}

versioned_database::~versioned_database() = default;

void versioned_database::open(std::string_view path,
                              int current_version,
                              const update_funct_t &update_func,
                              std::string_view version_table,
                              std::string_view version_column) {
   std::error_code ec;
   open(path, current_version, update_func, ec, version_table, version_column);
   if (ec) {
      throw std::system_error(ec);
   }
}

void versioned_database::open(std::string_view path,
                              int current_version,
                              const update_funct_t &update_func,
                              std::error_code &ec,
                              std::string_view version_table,
                              std::string_view version_column) noexcept {
   con_.open(path, ec);
   if (ec) {
      return;
   }

   auto file_version = get_file_version(version_table, version_column, ec);
   if (ec) {
      return;
   }

   if (file_version < current_version) {
      perform_update(file_version, current_version, update_func, version_table, version_column, ec);
   }
}

void versioned_database::prepare_update_statement(std::string_view table,
                                                  std::string_view column,
                                                  std::error_code &ec) {
   std::ostringstream os;
   os << "UPDATE " << table << " SET " << column << " = :pversion;";
   const auto sql = os.str();

   update_stmt_.prepare(sql, ec);
   update_prepared_ = true;
}

int versioned_database::get_file_version(std::string_view table, std::string_view column, std::error_code &ec) {
   statement meta_select{con_};
   meta_select.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:table;", ec);
   if (ec) {
      return 0;
   }

   meta_select.bind(1, table, ec);
   if (ec) {
      return 0;
   }

   if (!meta_select.step(ec)) {
      // No such table
      return 0;
   }

   // Construct version select statement
   std::ostringstream os;
   os << "SELECT " << column << " FROM " << table << ";";
   const auto sql = os.str();

   statement version{con_};
   version.prepare(sql, ec);
   if (ec) {
      return 0;
   }

   bool have_version = version.step(ec);
   if (ec || !have_version) {
      return 0;
   }

   int result;
   version.get(0, result, ec);
   return result;
}

void versioned_database::store_new_version(int version, std::error_code &ec) {
   update_stmt_.reset(ec);
   if (ec) {
      return;
   }

   update_stmt_.bind(1, version, ec);
   if (ec) {
      return;
   }

   update_stmt_.execute(ec);
   if (ec) {
      return;
   }
}

void versioned_database::perform_update(int from,
                                        int to,
                                        const update_funct_t &func,
                                        std::string_view table,
                                        std::string_view column,
                                        std::error_code &ec) {
   for (int current = from; current < to && !ec;) {
      func(*this, current, ec);
      if (ec) {
         return;
      }

      if (!update_prepared_) {
         prepare_update_statement(table, column, ec);
         if (ec) {
            return;
         }
      }

      ++current;

      store_new_version(current, ec);
   }
}

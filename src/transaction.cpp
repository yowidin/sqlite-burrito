/**
 * @file   transaction.cpp
 * @author Dennis Sitelew
 * @date   May 19, 2023
 */

#include <sqlite-burrito/transaction.h>
#include <sqlite-burrito/statement.h>

using namespace sqlite_burrito;

transaction::transaction(connection &con, behavior behavior)
   : con_{&con} {
   const char *stmt;
   switch (behavior) {
      case behavior::deferred:
         stmt = "BEGIN DEFERRED";
         break;

      case behavior::immediate:
         stmt = "BEGIN IMMEDIATE";
         break;

      case behavior::exclusive:
         stmt = "BEGIN EXCLUSIVE";
         break;

      default:
         throw std::system_error(std::make_error_code(std::errc::invalid_argument));
   }

   statement::execute(*con_, stmt);
}

transaction::~transaction() {
   if (rolled_back_ || committed_) {
      return;
   }

   // TODO: Handle rollback errors to avoid throwing from the destructor?
   statement::execute(*con_, "ROLLBACK TRANSACTION");
}

void transaction::commit() {
   std::error_code ec;
   commit(ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void transaction::commit(std::error_code &ec) noexcept {
   if (committed_ || rolled_back_) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   statement::execute(*con_, "COMMIT TRANSACTION", ec);
   committed_ = true;
}

void transaction::rollback() {
   std::error_code ec;
   rollback(ec);
   if (ec) {
      throw std::system_error(ec);
   }
}

void transaction::rollback(std::error_code &ec) noexcept {
   if (committed_ || rolled_back_) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return;
   }

   statement::execute(*con_, "ROLLBACK TRANSACTION", ec);
   rolled_back_ = true;
}

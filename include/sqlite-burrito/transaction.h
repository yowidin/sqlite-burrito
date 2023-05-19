/**
 * @file   transaction.h
 * @author Dennis Sitelew
 * @date   May 19, 2023
 */

#ifndef INCLUDE_SQLITE_BURRITO_TRANSACTION_H
#define INCLUDE_SQLITE_BURRITO_TRANSACTION_H

#include <sqlite-burrito/export.h>

#include <system_error>

namespace sqlite_burrito {

class connection;

class SQLITE_BURRITO_EXPORT transaction {
public:
   //! Transaction behavior, see https://www.sqlite.org/lang_transaction.html for more details
   enum class behavior {
      deferred,
      immediate,
      exclusive,
      default_behavior = deferred,
   };

public:
   transaction(connection &con, behavior behavior = behavior::default_behavior);

   transaction(transaction &) = delete;
   transaction(transaction &&) = default;

   ~transaction();

public:
   transaction &operator=(transaction &) = delete;
   transaction &operator=(transaction &&) = default;

public:
   void commit();
   void commit(std::error_code &ec) noexcept;

   void rollback();
   void rollback(std::error_code &ec) noexcept;

private:
   connection *con_;

   bool committed_{false};
   bool rolled_back_{false};
};

} // namespace sqlite_burrito

#endif // INCLUDE_SQLITE_BURRITO_TRANSACTION_H

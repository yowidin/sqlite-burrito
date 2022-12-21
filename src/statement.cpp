/**
 * @file   statement.cpp
 * @author Dennis Sitelew
 * @date   Jul. 22, 2020
 */

#include <sqlite-burrito/connection.h>
#include <sqlite-burrito/detail/connection_ptr.h>
#include <sqlite-burrito/detail/statement_ptr.h>
#include <sqlite-burrito/statement.h>

#include <iterator>

#include <sqlite3.h>

using namespace sqlite_burrito;

statement::statement(connection &conn, prepare_flags flags)
   : stmt_{new detail::statement_ptr()}
   , connection_{&conn}
   , flags_{flags} {
   // Nothing to do here
}

statement::~statement() = default;

statement::iterator_t statement::prepare(const std::string &text) {
   std::error_code ec;
   auto res = prepare(text, ec);
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

statement::iterator_t statement::prepare(const std::string &text,
                                         std::error_code &ec) noexcept {
   detail::statement_ptr::pointer ptr;
   const char *tail = nullptr;
   auto res = ::sqlite3_prepare_v3(connection_->get_native_wrapper().get(),
                                   text.data(), text.length(),
                                   static_cast<unsigned>(flags_), &ptr, &tail);
   ec = sqlite_burrito::connection::to_error_code(res);

   auto distance = std::distance(text.data(), tail);

   if (!ec) {
      stmt_->reset(ptr);
   }

   if (distance < text.length()) {
      auto result = std::begin(text);
      std::advance(result, distance);
      return result;
   } else {
      return std::end(text);
   }
}

std::string statement::execute(
    connection &conn,
    const char *sql,
    std::error_code &ec,
    exec_callback_t callback) noexcept(noexcept(std::string())) {
   std::string result;
   char *errorMessage{nullptr};
   exec_callback_wrapper wrapper{std::move(callback)};

   auto res =
       ::sqlite3_exec(conn.get_native_wrapper().get(), sql,
                      &exec_callback_wrapper::call, &wrapper, &errorMessage);

   if (errorMessage != nullptr) {
      result = errorMessage;
      sqlite3_free(errorMessage);
   }

   ec = sqlite_burrito::connection::to_error_code(res);
   return result;
}

std::string statement::execute(connection &conn,
                               const char *sql,
                               exec_callback_t callback) {
   std::error_code ec;
   auto res = execute(conn, sql, ec, std::move(callback));
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

std::string statement::execute(
    connection &conn,
    const char *sql,
    std::error_code &ec) noexcept(noexcept(std::string())) {
   std::string result;
   char *errorMessage{nullptr};

   auto res = ::sqlite3_exec(conn.get_native_wrapper().get(), sql, nullptr,
                             nullptr, &errorMessage);

   if (errorMessage != nullptr) {
      result = errorMessage;
      sqlite3_free(errorMessage);
   }

   ec = sqlite_burrito::connection::to_error_code(res);
   return result;
}

std::string statement::execute(connection &conn, const char *sql) {
   std::error_code ec;
   auto res = execute(conn, sql, ec);
   if (ec) {
      throw std::system_error(ec);
   }
   return res;
}

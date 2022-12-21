/**
 * @file   bind_parameters.cpp
 * @author Dennis Sitelew
 * @date   Jul. 23, 2020
 */

#include <sqlite-burrito/bind_parameters.h>
#include <sqlite-burrito/detail/statement_ptr.h>
#include <sqlite-burrito/statement.h>
#include <sqlite3.h>

using namespace sqlite_burrito;

std::string bind_parameters::name_getter::operator()(const statement &stmt,
                                                     int index) const {
   auto *ptr = stmt.get_native_wrapper().get();
   const char *text = ::sqlite3_bind_parameter_name(ptr, index);
   return (text != nullptr) ? std::string{text} : std::string{};
}

bind_parameters::bind_parameters(statement &stmt)
   : stmt_{&stmt}
   , parameter_count_{
         ::sqlite3_bind_parameter_count(stmt_->get_native_wrapper().get())} {
   // Nothing to do here
}

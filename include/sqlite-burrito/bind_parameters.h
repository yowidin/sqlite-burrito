/**
 * @file   bind_parameters.h
 * @author Dennis Sitelew
 * @date   Jul. 23, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_BIND_PARAMETERS_H
#define INCLUDE_SQLITE_BURRITO_BIND_PARAMETERS_H

#include <sqlite-burrito/detail/iterator.h>

#include <iterator>
#include <string>

namespace sqlite_burrito {

class statement;

class bind_parameters {
private:
   struct name_getter {
      std::string operator()(const statement &stmt, int index) const;
   };

public:
   using iterator = index_based_iterator<std::string, name_getter, statement, bind_parameters>;

public:
   bind_parameters(statement &stmt);

public:
   iterator begin() const { return iterator{1, *stmt_}; }
   iterator end() const { return iterator{parameter_count_ + 1, *stmt_}; }

   iterator cbegin() const { return begin(); }
   iterator cend() const { return end(); }

private:
   //! Statement, this set of bind parameters belongs to
   const statement *const stmt_;

   //! Cached number of parameters in the statement
   const int parameter_count_;
};

} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_BIND_PARAMETERS_H */

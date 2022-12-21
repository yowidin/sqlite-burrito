/**
 * @file   column.h
 * @author Dennis Sitelew
 * @date   Jul. 13, 2020
 */
#ifndef INCLUDE_COLUMN_H
#define INCLUDE_COLUMN_H

#include <sqlite-burrito/isolation/sqlite3.h>

#include <string>

namespace sqlite_burrito {

class statement;

class column {
public:
   //! Statement column types
   enum class type_t : int {
      int_t = isolation::sqlite_integer,
      text_t = isolation::sqlite3_text,
      float_t = isolation::sqlite_float,
      blob_t = isolation::sqlite_blob,
      null_t = isolation::sqlite_null,
   };

public:
   column(statement &stmt, int index);

   column(column &) = delete;
   column(column &&) = default;

public:
   column &operator=(column &) = delete;
   column &operator=(column &&) = default;

   int index() const { return index_; }
   type_t type() const { return type_; }
   const std::string &name() const { return name_; }

private:
   statement *stmt_;  //!< Statement, this column belongs to
   int index_;        //!< Column index in the statement
   type_t type_;      //!< Type of the column
   std::string name_; //!< Name of the column
};

} // namespace sqlite_burrito

#endif /* INCLUDE_COLUMN_H */

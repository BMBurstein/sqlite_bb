#pragma once

#include "sqlite3_statement.hpp"

#include <memory>
#include <stdexcept>
#include <string>

struct sqlite3;

namespace bb {
namespace sqlite {

class connection {
  std::shared_ptr<sqlite3> m_sqlite;

public:
  connection(std::string path = "");

  statement prepare(std::string const& sql);
}; // class connection


/************************************************************************/
/* Error types                                                          */
/************************************************************************/

class sqlite_error : public std::runtime_error {
public:
  explicit sqlite_error(char const* str) : std::runtime_error(str) {}
}; // class sqlite_error

class sqlite_misuse_error : public sqlite_error {
public:
  explicit sqlite_misuse_error(char const* str) : sqlite_error(str) {}
}; // class sqlite_misuse_error

} // namespace sqlite
} // namespace bb

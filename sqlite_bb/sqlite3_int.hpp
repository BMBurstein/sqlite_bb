#pragma once

#include "sqlite.hpp"
#include "sqlite3.h"

namespace bb {
namespace sqlite {

class TestMutex {
  sqlite3* m_db;
public:
  TestMutex() : m_db(nullptr) {}
  TestMutex(sqlite3* db) : m_db(db) { sqlite3_mutex_enter(sqlite3_db_mutex(m_db)); }
  TestMutex(sqlite3_stmt* stmt) : TestMutex(sqlite3_db_handle(stmt)) {}
  TestMutex(TestMutex const&) = delete;
  TestMutex(TestMutex&&) = default;
  TestMutex& operator=(TestMutex const&) = delete;
  TestMutex& operator=(TestMutex&&) = default;
  ~TestMutex() { sqlite3_mutex_leave(sqlite3_db_mutex(m_db)); }

  void test() {
    int e = sqlite3_errcode(m_db);
    if (e != SQLITE_OK && e != SQLITE_ROW && e != SQLITE_DONE) {
      switch (e) {
      case SQLITE_MISUSE:
        throw sqlite_misuse_error(sqlite3_errmsg(m_db));
      default:
        throw sqlite_error(sqlite3_errmsg(m_db));
      }
    }
  }
};

} // namespace sqlite
} // namespace bb

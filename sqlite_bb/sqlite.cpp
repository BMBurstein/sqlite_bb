#include "sqlite.hpp"

#include "sqlite3_int.hpp"

namespace bb {
namespace sqlite {

connection::connection(std::string path) {
  sqlite3* p;
  sqlite3_open(path.c_str(), &p);
  m_sqlite.reset(p, sqlite3_close_v2);
  TestMutex mtx(m_sqlite.get());
  mtx.test();
}

statement connection::prepare(std::string const& sql) {
  return statement(sql, m_sqlite.get());
}

} // namespace sqlite
} // namespace bb

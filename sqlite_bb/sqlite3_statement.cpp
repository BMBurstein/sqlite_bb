#include "sqlite3_statement.hpp"
#include "sqlite3_int.hpp"

namespace bb {
namespace sqlite {

//////////////////////////////////////////////////////////////////////////
// class statement
//////////////////////////////////////////////////////////////////////////

statement::statement(std::string const& sql, sqlite3* db) : m_stmt(nullptr), params(*this) {
  {
    TestMutex mtx(db);
    sqlite3_prepare_v2(db, sql.c_str(), int(sql.length() + 1), &m_stmt, nullptr);
    mtx.test();
  }
  m_params.reserve(num_params());
  for (int i = 0; i < num_params(); ++i) {
    m_params.emplace_back(this, i+1);
  }
}

statement::statement(statement const& other) : statement(sqlite3_sql(other.m_stmt), sqlite3_db_handle(other.m_stmt)) {

}

statement::statement(statement&& other) noexcept : m_stmt(nullptr), params(*this) {
  swap(*this, other);
}

statement& statement::operator=(statement other) noexcept {
  swap(*this, other);
  return *this;
}

statement::~statement() {
  sqlite3_finalize(m_stmt);
}

statement::row statement::begin() {
  sqlite3_reset(m_stmt);
  return row(this);
}

statement::row statement::end() {
  return row(this, SQLITE_DONE);
}

void statement::reset() {
  sqlite3_clear_bindings(m_stmt);
  sqlite3_reset(m_stmt);
}

int statement::num_columns() const {
  return sqlite3_column_count(m_stmt);
}

int statement::num_params() const {
  return sqlite3_bind_parameter_count(m_stmt);
}

//////////////////////////////////////////////////////////////////////////
// class statement::row
//////////////////////////////////////////////////////////////////////////

statement::row::row(statement* stmt) : m_stmt(stmt->m_stmt) {
  m_cols.reserve(stmt->num_columns());
  for (int i = 0; i < stmt->num_columns(); ++i) {
    m_cols.emplace_back(*this, i);
  }
  TestMutex mtx(m_stmt);
  m_step_res = sqlite3_step(m_stmt);
  mtx.test();
}

statement::row::column const& statement::row::operator[](int col) const {
  return m_cols[col];
}

statement::row& statement::row::operator++() {
  TestMutex mtx(m_stmt);
  m_step_res = sqlite3_step(m_stmt);
  mtx.test();
  return *this;
}

//////////////////////////////////////////////////////////////////////////
// class statement::row::column
//////////////////////////////////////////////////////////////////////////

statement::row::column::column(row& r, int col) : m_num(col), m_row(r) {

}

int32_t statement::row::column::get_int32() const {
  return sqlite3_column_int(m_row.m_stmt, m_num);
}

int64_t statement::row::column::get_int64() const {
  return sqlite3_column_int64(m_row.m_stmt, m_num);
}

double statement::row::column::get_double() const {
  return sqlite3_column_double(m_row.m_stmt, m_num);
}

std::string statement::row::column::get_string() const {
  auto str = (const char*)sqlite3_column_blob(m_row.m_stmt, m_num);
  auto len = sqlite3_column_bytes(m_row.m_stmt, m_num);
  return std::string(str, len);
}

VALUE_TYPES statement::row::column::get_type() const {
  return static_cast<VALUE_TYPES>(sqlite3_column_type(m_row.m_stmt, m_num));
}

//////////////////////////////////////////////////////////////////////////
// class statement::param
//////////////////////////////////////////////////////////////////////////

void statement::param::set_int32(int32_t v) {
  TestMutex mtx(m_stmt);
  sqlite3_bind_int(m_stmt, m_num, v);
  mtx.test();
}
void statement::param::set_int64(int64_t v) {
  TestMutex mtx(m_stmt);
  sqlite3_bind_int64(m_stmt, m_num, v);
  mtx.test();
}
void statement::param::set_double(double v) {
  TestMutex mtx(m_stmt);
  sqlite3_bind_double(m_stmt, m_num, v);
  mtx.test();
}
void statement::param::set_string(std::string const& v) {
  TestMutex mtx(m_stmt);
  sqlite3_bind_text(m_stmt, m_num, v.c_str(), int(v.size()), SQLITE_TRANSIENT);
  mtx.test();
}
void statement::param::set_null() {
  TestMutex mtx(m_stmt);
  sqlite3_bind_null(m_stmt, m_num);
  mtx.test();
}

//////////////////////////////////////////////////////////////////////////
// struct statement::param_proxy
//////////////////////////////////////////////////////////////////////////

statement::param & statement::param_proxy::operator[](std::string const& name) {
  return operator[](sqlite3_bind_parameter_index(m_stmt.m_stmt, name.c_str()));
}

} // namespace sqlite
} // namespace bb

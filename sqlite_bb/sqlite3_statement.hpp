#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

#include "sqlite3_detail.hpp"

namespace bb {
namespace sqlite {

enum class VALUE_TYPES {
  INT    = 1, // SQLITE_INTEGER
  DOUBLE = 2, // SQLITE_FLOAT
  STRING = 3, // SQLITE_TEXT
  BLOB   = 4, // SQLITE_BLOB
  NULL_T = 5, // SQLITE_NULL
};

class statement {
  sqlite3_stmt* m_stmt;

  class row {
    sqlite3_stmt* m_stmt;
    int m_step_res;

    class column {
      int m_num;
      row& m_row;
    public:
      column(row& r, int col);
      column(column const&) = delete;
      column& operator=(column const&) = delete;
      column(column&&) = default;
      column& operator=(column&&) = default;

      int32_t get_int32() const;
      int64_t get_int64() const;
      double  get_double() const;
      std::string get_string() const;
#ifndef DEFAULT_INT64
      int32_t get_int() const { return get_int32(); }
#else
      int64_t get_int() const { return get_int64(); }
#endif

      operator int32_t() const { return get_int32(); }
      operator int64_t() const { return get_int64(); }
      operator double() const { return get_double(); }
      operator std::string() const { return get_string(); }

      VALUE_TYPES get_type() const;
    };

    std::vector<column> m_cols;

  public:
    row(statement* stmt);
    row(statement* stmt, int step_res) : m_stmt(stmt->m_stmt), m_step_res(step_res) {}
    row(row const&) = delete;
    row& operator=(row const&) = delete;
    row(row&&) = default;
    row& operator=(row&&) = default;

    std::vector<column> const& operator*() const { return m_cols; }
    column const& operator[](int col) const;
    row& operator++();
    bool operator==(row const& rhs) { return m_step_res == rhs.m_step_res; }
    bool operator!=(row const& rhs) { return !(*this == rhs); }
  };

  class param {
    int m_num;
    sqlite3_stmt* m_stmt;
  public:
    param(statement* stmt, int param) : m_stmt(stmt->m_stmt), m_num(param) {}
    param(param const&) = delete;
    param& operator=(param const&) = delete;
    param(param&&) = default;
    param& operator=(param&&) = default;

    void set_int32(int32_t);
    void set_int64(int64_t);
    void set_double(double);
    void set_string(std::string const&);
    void set_null();
#ifndef DEFAULT_INT64
    void set_int(int32_t v) { set_int32(v); }
#else
    void set_int(int64_t v) { set_int64(v); }
#endif

    void set(int32_t v) { set_int32(v); }
    void set(int64_t v) { set_int64(v); }
    void set(double v) { set_double(v); }
    void set(std::string const& v) { set_string(v); }
    void set() { set_null(); }

    template<class T>
    param& operator=(T v) { set(v); return *this; }
  };

  std::vector<param> m_params;

  class param_proxy {
    statement& m_stmt;
  public:
    param_proxy(statement& stmt) : m_stmt(stmt) {}
    param& operator[](int num) { return m_stmt.m_params[num-1]; }
    param& operator[](std::string const& name);
  };

public:
  statement(std::string const& sql, sqlite3* db);
  statement(statement const&);
  statement(statement&&) noexcept;
  statement& operator=(statement) noexcept;
  ~statement();

  friend void swap(statement& stmt1, statement& stmt2) noexcept {
    using std::swap;
    swap(stmt1.m_stmt, stmt2.m_stmt);
    swap(stmt1.m_params, stmt2.m_params);
    //swap(stmt1.m_param_proxy, stmt2.m_param_proxy);
  }

  row begin();
  row end();

  void reset();

  template <class Func>
  void run(Func callback) {
    for (auto& it : *this) {
      detail::call(callback, it, std::make_index_sequence<detail::function_traits<Func>::arity>());
    }
  }

  int num_columns() const;
  int num_params() const;

  param_proxy params;
}; // class statement

} // namespace sqlite
} // namespace bb

#include "../sqlite_bb/sqlite.hpp"

#include "catch.hpp"

using namespace bb::sqlite;

TEST_CASE("Connection creation", "[connection]") {
  SECTION("basic") {
    REQUIRE_NOTHROW([] {
      connection db;
    }());
  }
  SECTION("memory") {
    REQUIRE_NOTHROW([] {
      connection db(":memory:");
    }());
  }
  SECTION("temp file") {
    REQUIRE_NOTHROW([] {
      connection db("");
    }());
  }
  SECTION("bad file name") {
    REQUIRE_THROWS_AS([] {
      connection db("////");
    }(), sqlite_error);
  }
}

TEST_CASE("Statment preperation", "[statement]") {
  connection db;

  SECTION("Prepare") {
    db.prepare("SELECT 1;");
  }
  SECTION("Prepare empty") {
    db.prepare("");
  }
  SECTION("Prepare non-sql") {
    REQUIRE_THROWS_AS(db.prepare("Hello world!"), sqlite_error);
  }
  SECTION("Prepare sql error") {
    REQUIRE_THROWS_AS(db.prepare("SELECT 1 FROM non_existing_table"), sqlite_error);
  }
}
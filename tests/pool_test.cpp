#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "detail/pool.hpp"

using fysics::detail::Pool;
using IntPool = Pool<int>;

TEST_CASE("Pool add returns a valid id for the stored value", "[pool]") {
  IntPool pool;
  const IntPool::Id id = pool.add(42);

  REQUIRE(pool.is_valid(id));
  REQUIRE(pool.get(id) == 42);
  REQUIRE(pool.at(id) == 42);
}

TEST_CASE("Pool items are mutable through get and at", "[pool]") {
  IntPool pool;
  const IntPool::Id id = pool.add(1);

  pool.get(id) = 2;
  REQUIRE(pool.at(id) == 2);

  pool.at(id) = 3;
  REQUIRE(pool.get(id) == 3);
}

TEST_CASE("Pool add gives distinct ids", "[pool]") {
  IntPool pool;
  const IntPool::Id a = pool.add(1);
  const IntPool::Id b = pool.add(2);
  const IntPool::Id c = pool.add(3);

  REQUIRE(a.id != b.id);
  REQUIRE(b.id != c.id);
  REQUIRE(a.id != c.id);
  REQUIRE(pool.at(a) == 1);
  REQUIRE(pool.at(b) == 2);
  REQUIRE(pool.at(c) == 3);
}

TEST_CASE("Pool add beyond the initial capacity keeps earlier items", "[pool]") {
  IntPool pool{2};
  const IntPool::Id a = pool.add(1);
  const IntPool::Id b = pool.add(2);
  const IntPool::Id c = pool.add(3);

  REQUIRE(pool.at(a) == 1);
  REQUIRE(pool.at(b) == 2);
  REQUIRE(pool.at(c) == 3);
}

TEST_CASE("Pool erase invalidates the id", "[pool]") {
  IntPool pool;
  const IntPool::Id id = pool.add(7);
  const IntPool::Id other = pool.add(8);

  pool.erase(id);

  REQUIRE_FALSE(pool.is_valid(id));
  REQUIRE_THROWS_AS(pool.at(id), std::out_of_range);

  SECTION("other items are unaffected") {
    REQUIRE(pool.is_valid(other));
    REQUIRE(pool.at(other) == 8);
  }

  SECTION("erasing again is a no-op") {
    pool.erase(id);
    // a double erase must not free the slot twice, so two adds get two slots
    const IntPool::Id a = pool.add(1);
    const IntPool::Id b = pool.add(2);
    REQUIRE(a.id != b.id);
    REQUIRE(pool.at(a) == 1);
    REQUIRE(pool.at(b) == 2);
  }
}

TEST_CASE("Pool reuses freed slots with a new generation", "[pool]") {
  IntPool pool;
  const IntPool::Id old_id = pool.add(1);
  pool.erase(old_id);

  const IntPool::Id new_id = pool.add(2);

  REQUIRE(new_id.id == old_id.id);
  REQUIRE(new_id.generation == old_id.generation + 1);
  REQUIRE(pool.at(new_id) == 2);

  SECTION("the stale id stays invalid") {
    REQUIRE_FALSE(pool.is_valid(old_id));
    REQUIRE_THROWS_AS(pool.at(old_id), std::out_of_range);
  }

  SECTION("erasing through the stale id does nothing") {
    pool.erase(old_id);
    REQUIRE(pool.is_valid(new_id));
    REQUIRE(pool.at(new_id) == 2);
  }
}

TEST_CASE("Pool rejects ids that were never allocated", "[pool]") {
  IntPool pool;
  pool.add(1);

  const IntPool::Id out_of_range{5, 0};
  REQUIRE_FALSE(pool.is_valid(out_of_range));
  REQUIRE_THROWS_AS(pool.at(out_of_range), std::out_of_range);

  const IntPool::Id wrong_generation{0, 1};
  REQUIRE_FALSE(pool.is_valid(wrong_generation));
}

TEST_CASE("Pool on an empty pool has no valid ids", "[pool]") {
  const IntPool pool;
  REQUIRE_FALSE(pool.is_valid(IntPool::Id{0, 0}));
}

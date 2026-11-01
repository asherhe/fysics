#include <catch2/catch_test_macros.hpp>

#include "fysics/math.hpp"

using fysics::Vec2;

TEST_CASE("Vec2 default constructs to zero", "[math][vec2]") {
  constexpr Vec2 v;
  STATIC_REQUIRE(v.x == 0.0f);
  STATIC_REQUIRE(v.y == 0.0f);
}

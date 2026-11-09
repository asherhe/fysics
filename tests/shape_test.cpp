#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "fysics/types.hpp"

using Catch::Approx;
using fysics::AABB;
using fysics::BoxShape;
using fysics::CircleShape;
using fysics::Mat23;
using fysics::Shape;
using fysics::Vec2;

namespace {

void require_aabb(const AABB &actual, Vec2 min, Vec2 max) {
  REQUIRE(actual.min.x == Approx(min.x));
  REQUIRE(actual.min.y == Approx(min.y));
  REQUIRE(actual.max.x == Approx(max.x));
  REQUIRE(actual.max.y == Approx(max.y));
}

} // namespace

TEST_CASE("CircleShape exposes its physical properties", "[shape][circle]") {
  const CircleShape circle{2.0f};

  REQUIRE(circle.center_of_mass().x == 0.0f);
  REQUIRE(circle.center_of_mass().y == 0.0f);
  REQUIRE(circle.moment_of_inertia() == 2.0f);
}

TEST_CASE("CircleShape bounds use the transform translation", "[shape][circle][aabb]") {
  const CircleShape circle{2.0f};

  require_aabb(circle.aabb(Mat23::rot_trans(1.2f, {3.0f, -4.0f})), {1.0f, -6.0f}, {5.0f, -2.0f});
}

TEST_CASE("CircleShape supports the zero-radius boundary", "[shape][circle][aabb]") {
  const CircleShape point{0.0f};
  require_aabb(point.aabb(Mat23::rot_trans(0.5f, {3.0f, -4.0f})), {3.0f, -4.0f}, {3.0f, -4.0f});
  REQUIRE(point.moment_of_inertia() == 0.0f);
}

TEST_CASE("BoxShape exposes its physical properties", "[shape][box]") {
  const BoxShape box{{6.0f, 4.0f}};

  REQUIRE(box.center_of_mass().x == 0.0f);
  REQUIRE(box.center_of_mass().y == 0.0f);
  REQUIRE(box.moment_of_inertia() == Approx(52.0f / 12.0f));
}

TEST_CASE("BoxShape bounds cover unrotated and rotated corners", "[shape][box][aabb]") {
  const BoxShape box{{6.0f, 4.0f}};

  SECTION("unrotated") {
    require_aabb(box.aabb(Mat23::rot_trans(0.0f, {1.0f, -2.0f})), {-2.0f, -4.0f}, {4.0f, 0.0f});
  }

  SECTION("quarter turn swaps extents") {
    require_aabb(
        box.aabb(Mat23::rot_trans(1.57079632679f, {1.0f, -2.0f})), {-1.0f, -5.0f}, {3.0f, 1.0f}
    );
  }
}

TEST_CASE("BoxShape supports zero dimensions", "[shape][box][aabb]") {
  const BoxShape point{{0.0f, 0.0f}};
  require_aabb(point.aabb(Mat23::rot_trans(0.7f, {3.0f, -4.0f})), {3.0f, -4.0f}, {3.0f, -4.0f});
  REQUIRE(point.moment_of_inertia() == 0.0f);
}

TEST_CASE("Shape dispatch preserves concrete-shape results", "[shape][variant]") {
  const Shape circle = CircleShape{2.0f};
  const Shape box = BoxShape{{6.0f, 4.0f}};
  const Mat23 transform = Mat23::rot_trans(0.0f, {1.0f, -2.0f});

  REQUIRE(fysics::shape_center_of_mass(circle).x == 0.0f);
  REQUIRE(fysics::shape_moment_of_inertia(circle) == 2.0f);
  require_aabb(fysics::shape_aabb(circle, transform), {-1.0f, -4.0f}, {3.0f, 0.0f});

  REQUIRE(fysics::shape_center_of_mass(box).y == 0.0f);
  REQUIRE(fysics::shape_moment_of_inertia(box) == Approx(52.0f / 12.0f));
  require_aabb(fysics::shape_aabb(box, transform), {-2.0f, -4.0f}, {4.0f, 0.0f});
}

#include <optional>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "fysics/fysics.hpp"
#include "fysics/math.hpp"
#include "fysics/types.hpp"

#include "detail/contact.hpp"
#include "detail/world_impl.hpp"

using Catch::Approx;
using fysics::BodyConfig;
using fysics::BoxShape;
using fysics::CircleShape;
using fysics::Mat23;
using fysics::Shape;
using fysics::SimulationType;
using fysics::Vec2;
using fysics::World;
using fysics::WorldConfig;
using fysics::detail::Contact;
using fysics::detail::WorldImplAccess;

namespace {

// a body-to-world transform that places a shape at pos without rotation
Mat23 at(Vec2 pos) {
  return Mat23::rot_trans(0.0f, pos);
}

void require_vec(const Vec2 &actual, Vec2 expected) {
  REQUIRE(actual.x == Approx(expected.x));
  REQUIRE(actual.y == Approx(expected.y));
}

BodyConfig body_at(Vec2 pos, const Shape &shape, SimulationType sim = SimulationType::Dynamic) {
  BodyConfig config;
  config.pos = pos;
  config.shape = shape;
  config.sim_type = sim;
  return config;
}

} // namespace

TEST_CASE("shape_detect_collision reports overlapping circles", "[detect][circle]") {
  const Shape a = CircleShape{1.0f};
  const Shape b = CircleShape{1.0f};

  const auto contact = fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({1.5f, 0.0f}));

  REQUIRE(contact.has_value());
  require_vec(contact->normal, {1.0f, 0.0f});
  require_vec(contact->pos, {1.0f, 0.0f});
  REQUIRE(contact->depth == Approx(0.5f));
  REQUIRE(contact->bodyA == nullptr);
  REQUIRE(contact->bodyB == nullptr);
}

TEST_CASE("shape_detect_collision separates distant circles", "[detect][circle]") {
  const Shape a = CircleShape{1.0f};
  const Shape b = CircleShape{1.0f};

  REQUIRE_FALSE(
      fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({3.0f, 0.0f})).has_value()
  );
}

TEST_CASE("shape_detect_collision touching circles do not collide", "[detect][circle][boundary]") {
  const Shape a = CircleShape{1.0f};
  const Shape b = CircleShape{1.0f};

  // centres exactly two radii apart: separation is zero, which is not an overlap
  REQUIRE_FALSE(
      fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({2.0f, 0.0f})).has_value()
  );
}

TEST_CASE("shape_detect_collision resolves a circle against a box face", "[detect][circle][box]") {
  const Shape circle = CircleShape{1.0f};
  const Shape box = BoxShape{{2.0f, 2.0f}};

  // circle centre 1.5 to the right of a box centred at the origin (half-width 1)
  const auto contact =
      fysics::shape_detect_collision(circle, at({1.5f, 0.0f}), box, at({0.0f, 0.0f}));

  REQUIRE(contact.has_value());
  // normal points from the circle (a) towards the box (b)
  require_vec(contact->normal, {-1.0f, 0.0f});
  require_vec(contact->pos, {0.5f, 0.0f});
  REQUIRE(contact->depth == Approx(0.5f));
}

TEST_CASE("shape_detect_collision handles a circle centred inside a box", "[detect][circle][box]") {
  const Shape circle = CircleShape{0.5f};
  const Shape box = BoxShape{{4.0f, 2.0f}};

  // circle centre sits inside the box, closer to the top face (half-height 1)
  const auto contact =
      fysics::shape_detect_collision(circle, at({0.0f, 0.6f}), box, at({0.0f, 0.0f}));

  REQUIRE(contact.has_value());
  require_vec(contact->normal, {0.0f, 1.0f});
  // depth = radius + distance from centre to the nearest (top) face
  REQUIRE(contact->depth == Approx(0.5f + (1.0f - 0.6f)));
}

TEST_CASE("shape_detect_collision box-against-circle flips the normal", "[detect][box][circle]") {
  const Shape box = BoxShape{{2.0f, 2.0f}};
  const Shape circle = CircleShape{1.0f};

  const auto contact =
      fysics::shape_detect_collision(box, at({0.0f, 0.0f}), circle, at({1.5f, 0.0f}));

  REQUIRE(contact.has_value());
  // normal still points from a (box) towards b (circle)
  require_vec(contact->normal, {1.0f, 0.0f});
  require_vec(contact->pos, {0.5f, 0.0f});
  REQUIRE(contact->depth == Approx(0.5f));
}

TEST_CASE("shape_detect_collision separates a distant circle and box", "[detect][circle][box]") {
  const Shape circle = CircleShape{1.0f};
  const Shape box = BoxShape{{2.0f, 2.0f}};

  // gap between the circle surface and the box face
  REQUIRE_FALSE(
      fysics::shape_detect_collision(circle, at({3.0f, 0.0f}), box, at({0.0f, 0.0f})).has_value()
  );
}

TEST_CASE("shape_detect_collision reports overlapping boxes", "[detect][box]") {
  const Shape a = BoxShape{{2.0f, 2.0f}};
  const Shape b = BoxShape{{2.0f, 2.0f}};

  const auto contact = fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({1.5f, 0.0f}));

  REQUIRE(contact.has_value());
  require_vec(contact->normal, {1.0f, 0.0f});
  REQUIRE(contact->depth == Approx(0.5f));
  // contact point is the corner of b nearest a's centre
  require_vec(contact->pos, {0.5f, -1.0f});
}

TEST_CASE("shape_detect_collision picks the axis of least penetration", "[detect][box]") {
  const Shape a = BoxShape{{2.0f, 2.0f}};
  const Shape b = BoxShape{{2.0f, 2.0f}};

  // shallower overlap on y than on x, so the normal resolves along y
  const auto contact = fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({0.5f, 1.5f}));

  REQUIRE(contact.has_value());
  require_vec(contact->normal, {0.0f, 1.0f});
  REQUIRE(contact->depth == Approx(0.5f));
}

TEST_CASE("shape_detect_collision separates distant boxes", "[detect][box]") {
  const Shape a = BoxShape{{2.0f, 2.0f}};
  const Shape b = BoxShape{{2.0f, 2.0f}};

  REQUIRE_FALSE(
      fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, at({3.0f, 0.0f})).has_value()
  );
}

TEST_CASE("shape_detect_collision detects an overlap only on a rotated axis", "[detect][box]") {
  const Shape a = BoxShape{{2.0f, 2.0f}};
  const Shape b = BoxShape{{2.0f, 2.0f}};

  // axis-aligned, these boxes would be clear of each other on both world axes,
  // but a 45-degree rotation of b brings a corner into a
  const Mat23 xf_b = Mat23::rot_trans(0.785398163f, {2.2f, 0.0f});

  const auto contact = fysics::shape_detect_collision(a, at({0.0f, 0.0f}), b, xf_b);

  REQUIRE(contact.has_value());
  REQUIRE(contact->depth > 0.0f);
}

TEST_CASE("detect_collisions reports contacts between pooled bodies", "[detect][world]") {
  World world{WorldConfig{}};
  const auto a = world.create_body(body_at({0.0f, 0.0f}, CircleShape{1.0f}));
  const auto b = world.create_body(body_at({1.5f, 0.0f}, CircleShape{1.0f}));

  auto &impl = WorldImplAccess::get_impl(world);
  const std::vector<Contact> contacts = impl.detect_collisions();

  REQUIRE(contacts.size() == 1);
  // both bodies are filled in on the contact
  REQUIRE(contacts.front().bodyA != nullptr);
  REQUIRE(contacts.front().bodyB != nullptr);
  REQUIRE(contacts.front().bodyA != contacts.front().bodyB);
  REQUIRE(contacts.front().depth == Approx(0.5f));

  (void)a;
  (void)b;
}

TEST_CASE("detect_collisions ignores non-overlapping bodies", "[detect][world]") {
  World world{WorldConfig{}};
  world.create_body(body_at({0.0f, 0.0f}, CircleShape{1.0f}));
  world.create_body(body_at({5.0f, 0.0f}, CircleShape{1.0f}));

  auto &impl = WorldImplAccess::get_impl(world);
  REQUIRE(impl.detect_collisions().empty());
}

TEST_CASE("detect_collisions skips pairs of static bodies", "[detect][world][static]") {
  World world{WorldConfig{}};
  // overlapping, but both static: no contact should be produced
  world.create_body(body_at({0.0f, 0.0f}, CircleShape{1.0f}, SimulationType::Static));
  world.create_body(body_at({1.5f, 0.0f}, CircleShape{1.0f}, SimulationType::Static));

  auto &impl = WorldImplAccess::get_impl(world);
  REQUIRE(impl.detect_collisions().empty());
}

TEST_CASE("detect_collisions keeps a dynamic-static overlap", "[detect][world][static]") {
  World world{WorldConfig{}};
  world.create_body(body_at({0.0f, 0.0f}, CircleShape{1.0f}, SimulationType::Static));
  world.create_body(body_at({1.5f, 0.0f}, CircleShape{1.0f}, SimulationType::Dynamic));

  auto &impl = WorldImplAccess::get_impl(world);
  REQUIRE(impl.detect_collisions().size() == 1);
}

TEST_CASE("detect_collisions finds every colliding pair", "[detect][world]") {
  World world{WorldConfig{}};
  // three circles in a row, each overlapping only its immediate neighbour
  world.create_body(body_at({0.0f, 0.0f}, CircleShape{1.0f}));
  world.create_body(body_at({1.5f, 0.0f}, CircleShape{1.0f}));
  world.create_body(body_at({3.0f, 0.0f}, CircleShape{1.0f}));

  auto &impl = WorldImplAccess::get_impl(world);
  // pairs (0,1) and (1,2) overlap; (0,2) are 3.0 apart and do not
  REQUIRE(impl.detect_collisions().size() == 2);
}

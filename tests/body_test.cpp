#include <variant>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "detail/body.hpp"

using Catch::Approx;
using fysics::BodyConfig;
using fysics::BoxShape;
using fysics::CircleShape;
using fysics::SimulationType;
using fysics::detail::BodyImpl;

namespace {

BodyConfig configured_body() {
  BodyConfig config;
  config.pos = {1.0f, 2.0f};
  config.vel = {3.0f, 4.0f};
  config.angle = 0.25f;
  config.ang_vel = -0.5f;
  config.mass = 2.0f;
  config.shape = CircleShape{1.0f};
  return config;
}

} // namespace

TEST_CASE("BodyImpl is initialized from its configuration", "[body]") {
  const BodyConfig config = configured_body();
  const BodyImpl body{config};

  REQUIRE(body.get_pos().x == 1.0f);
  REQUIRE(body.get_pos().y == 2.0f);
  REQUIRE(body.get_vel().x == 3.0f);
  REQUIRE(body.get_vel().y == 4.0f);
  REQUIRE(body.get_angle() == 0.25f);
  REQUIRE(body.get_ang_vel() == -0.5f);
  REQUIRE(body.get_mass() == 2.0f);
  REQUIRE(body.get_sim_type() == SimulationType::Dynamic);
  REQUIRE(std::holds_alternative<CircleShape>(body.get_shape()));
  REQUIRE(std::get<CircleShape>(body.get_shape()).radius == 1.0f);
}

TEST_CASE("BodyImpl setters update every configurable property", "[body]") {
  BodyImpl body{configured_body()};

  body.set_pos({-1.0f, -2.0f});
  body.set_vel({-3.0f, -4.0f});
  body.set_angle(0.75f);
  body.set_ang_vel(1.5f);
  body.set_mass(4.0f);
  body.set_sim_type(SimulationType::Static);
  body.set_shape(BoxShape{{6.0f, 4.0f}});

  REQUIRE(body.get_pos().x == -1.0f);
  REQUIRE(body.get_pos().y == -2.0f);
  REQUIRE(body.get_vel().x == -3.0f);
  REQUIRE(body.get_vel().y == -4.0f);
  REQUIRE(body.get_angle() == Approx(0.75f));
  REQUIRE(body.get_ang_vel() == Approx(1.5f));
  REQUIRE(body.get_mass() == 4.0f);
  REQUIRE(body.get_sim_type() == SimulationType::Static);
  REQUIRE(std::holds_alternative<BoxShape>(body.get_shape()));
  const auto size = std::get<BoxShape>(body.get_shape()).size;
  REQUIRE(size.x == 6.0f);
  REQUIRE(size.y == 4.0f);
}

TEST_CASE("BodyImpl accepts zero-valued physical boundaries", "[body]") {
  BodyConfig config = configured_body();
  config.pos = {};
  config.vel = {};
  config.angle = 0.0f;
  config.ang_vel = 0.0f;
  config.mass = 0.0f;
  config.shape = BoxShape{{0.0f, 0.0f}};
  const BodyImpl body{config};

  REQUIRE(body.get_pos().x == 0.0f);
  REQUIRE(body.get_vel().y == 0.0f);
  REQUIRE(body.get_angle() == 0.0f);
  REQUIRE(body.get_ang_vel() == 0.0f);
  REQUIRE(body.get_mass() == 0.0f);
  REQUIRE(std::get<BoxShape>(body.get_shape()).size.x == 0.0f);
}

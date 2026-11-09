#include <stdexcept>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "fysics/fysics.hpp"

using fysics::BodyConfig;
using fysics::BoxShape;
using fysics::CircleShape;
using fysics::SimulationType;
using fysics::World;
using fysics::WorldConfig;

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

TEST_CASE("World creates bodies with their configured state", "[world]") {
  World world{WorldConfig{}};
  const auto body = world.create_body(configured_body());

  REQUIRE(body.get_pos().x == 1.0f);
  REQUIRE(body.get_pos().y == 2.0f);
  REQUIRE(body.get_vel().x == 3.0f);
  REQUIRE(body.get_vel().y == 4.0f);
  REQUIRE(body.get_angle() == 0.25f);
  REQUIRE(body.get_ang_vel() == -0.5f);
  REQUIRE(body.get_mass() == 2.0f);
  REQUIRE(body.get_sim_type() == SimulationType::Dynamic);
  REQUIRE(std::holds_alternative<CircleShape>(body.get_shape()));
}

TEST_CASE("Body setters update state through the World interface", "[world][body]") {
  World world{WorldConfig{}};
  auto body = world.create_body(configured_body());

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
  REQUIRE(body.get_angle() == 0.75f);
  REQUIRE(body.get_ang_vel() == 1.5f);
  REQUIRE(body.get_mass() == 4.0f);
  REQUIRE(body.get_sim_type() == SimulationType::Static);
  REQUIRE(std::holds_alternative<BoxShape>(body.get_shape()));
  const auto size = std::get<BoxShape>(body.get_shape()).size;
  REQUIRE(size.x == 6.0f);
  REQUIRE(size.y == 4.0f);
}

TEST_CASE("World reuses deleted body slots with a new generation", "[world][lifecycle]") {
  World world{WorldConfig{}};
  const auto first = world.create_body(configured_body());
  const auto second = world.create_body(configured_body());

  REQUIRE(first.id().id != second.id().id);
  REQUIRE(first.id().world_id == second.id().world_id);

  world.delete_body(first.id());
  REQUIRE_THROWS_AS(first.get_pos(), std::out_of_range);

  const auto replacement = world.create_body(configured_body());
  REQUIRE(replacement.id().id == first.id().id);
  REQUIRE(
      replacement.id().generation == static_cast<fysics::Generation>(first.id().generation + 1)
  );
  REQUIRE_THROWS_AS(world.get_body(first.id()).get_mass(), std::out_of_range);
}

TEST_CASE("Body access rejects ids from another World", "[world][validation]") {
  World first{WorldConfig{}};
  World second{WorldConfig{}};
  const auto body = first.create_body(configured_body());

  REQUIRE(first.get_body(body.id()).get_mass() == 2.0f);
  REQUIRE_THROWS_AS(second.get_body(body.id()).get_pos(), std::invalid_argument);
}

TEST_CASE("World stepping completes without contacts", "[world][step]") {
  World world{WorldConfig{}};
  auto body = world.create_body(configured_body());

  world.step(0.0f);
  world.step(1.0f);

  REQUIRE(body.get_pos().x == 1.0f);
  REQUIRE(body.get_vel().y == 4.0f);
}

#pragma once

#include <cstdint>
#include "math.hpp"

namespace fysics {

// unique identifier assigned to every fysics::Body
struct BodyId {
  uint32_t id;          // unique among loaded Bodies in a World
  uint16_t generation;  // generation of object in pool
  uint16_t world_id;    // identifier of World instance
};

// defines fysics::Body's behaviour in motion
//   SimulationType::Static  - position, orientation fixed in place
//   SimulationType::Dynamic - full simulation of collision response
enum class SimulationType { Static, Dynamic };

// configuration for creating a fysics::Body
struct BodyConfig {
  // position, m
  Vec2 pos{0.0f, 0.0f};
  // velocity, m/s
  Vec2 vel{0.0f, 0.0f};

  // angle, rad
  float angle = 0.0f;
  // angular velocity, rad/s
  float ang_vel = 0.0f;

  // mass, kg
  float mass = 1.0f;

  // rigidbody simulation type
  SimulationType sim_type = SimulationType::Static;
};


// maximum number of worlds that can be created
inline constexpr std::size_t MAX_WORLDS = UINT16_MAX;

// configuration for creating a fysics::World
struct WorldConfig {
  Vec2 gravity{0.0f, -9.8f};
};

} // namespace fysics

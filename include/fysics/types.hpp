#pragma once

#include <cstdint>
#include <variant>

#include "fysics/math.hpp"

namespace fysics {

// unique identifier assigned to every fysics::World
using Generation = std::uint16_t;
using WorldId = std::uint16_t;

// unique identifier assigned to every fysics::Body
struct BodyId {
  std::uint32_t id;      // unique among loaded Bodies in a World
  Generation generation; // generation of object in pool
  WorldId world_id;      // identifier of World instance
};


struct CircleShape {
  float radius;
};

struct BoxShape {
  // {x, y} dimensions of box
  Vec2 dimensions;
};

// describes the geometry of a fysics::Body. carries no information about the
// location of the shape in world space, only a definition of the body's shape.
//
// internally, fysics::Shape uses std::variant to discriminate between Shape
// subtypes (e.g. CircleShape, BoxShape, etc.), so that all the options can be
// encoded into one value.
using Shape = std::variant<CircleShape, BoxShape>;


// defines fysics::Body's behaviour in motion
//   SimulationType::Static  - position, orientation fixed in place
//   SimulationType::Dynamic - full simulation of collision response
enum class SimulationType : std::uint8_t { Static, Dynamic };

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
  SimulationType sim_type = SimulationType::Dynamic;

  // collider geometry of body
  Shape shape;
};


// maximum number of worlds that can be created
inline constexpr std::size_t MAX_WORLDS = UINT16_MAX;

// configuration for creating a fysics::World
struct WorldConfig {
  Vec2 gravity{0.0f, -9.8f};
};

} // namespace fysics

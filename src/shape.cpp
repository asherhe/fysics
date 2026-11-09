#include "fysics/types.hpp"

namespace fysics {

// circle and boxes are kinda trivial

Vec2 CircleShape::center_of_mass() const {
  return {0.0f, 0.0f};
}

float CircleShape::moment_of_inertia() const {
  return 0.5f * radius * radius;
}

Vec2 BoxShape::center_of_mass() const {
  return {0.0f, 0.0f};
}

float CircleShape::moment_of_inertia() const {
  return (dimensions.norm2) / 12.0f;
}

} // namespace fysics

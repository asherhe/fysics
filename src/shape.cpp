#include "fysics/types.hpp"

namespace fysics {

// circle and boxes are kinda trivial

Vec2 CircleShape::center_of_mass() const {
  return {0.0f, 0.0f};
}

Vec2 BoxShape::center_of_mass() const {
  return {0.0f, 0.0f};
}

} // namespace fysics

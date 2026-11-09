#include <variant>

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

float BoxShape::moment_of_inertia() const {
  return size.norm2() / 12.0f;
}

Vec2 shape_center_of_mass(const Shape &shape) {
  return std::visit([](auto &&s) { return s.center_of_mass(); }, shape);
}
float shape_moment_of_inertia(const Shape &shape) {
  return std::visit([](auto &&s) { return s.moment_of_inertia(); }, shape);
}

} // namespace fysics

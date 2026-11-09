#include <algorithm>
#include <cstdlib>
#include <variant>

#include "fysics/types.hpp"

using std::abs;
using std::max;

namespace fysics {

// circle and boxes are kinda trivial

Vec2 CircleShape::center_of_mass() const {
  return {0.0f, 0.0f};
}

float CircleShape::moment_of_inertia() const {
  return 0.5f * radius * radius;
}
AABB CircleShape::aabb(Mat23 transform) const {
  // TODO: assumes that transform is strictly orthonormal
  // if we add rigidbody scaling, we may need to generalize
  Vec2 size = Vec2(radius, radius), pos = transform.translation;
  return AABB{pos - size, pos + size};
}

Vec2 BoxShape::center_of_mass() const {
  return {0.0f, 0.0f};
}
float BoxShape::moment_of_inertia() const {
  return size.norm2() / 12.0f;
}
AABB BoxShape::aabb(Mat23 transform) const {
  Vec2 half_size = 0.5f * size;
  Vec2 v0 = transform.transform_vector(half_size),
       v1 = transform.transform_vector({half_size.x, -half_size.y});

  Vec2 bounds = {
    max(abs(v0.x), abs(v1.x)),
    max(abs(v0.y), abs(v1.y))
  };

  Vec2 pos = transform.translation;
  return AABB{pos - bounds, pos + bounds};
}

Vec2 shape_center_of_mass(const Shape &shape) {
  return std::visit([](auto &&s) { return s.center_of_mass(); }, shape);
}
float shape_moment_of_inertia(const Shape &shape) {
  return std::visit([](auto &&s) { return s.moment_of_inertia(); }, shape);
}
AABB shape_aabb(const Shape &shape, Mat23 transform) {
  return std::visit([transform](auto &&s) { return s.aabb(transform); }, shape);
}

} // namespace fysics

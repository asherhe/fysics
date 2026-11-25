#pragma once

#include <optional>

#include "body.hpp"

namespace fysics::detail {

struct Contact {
  BodyImpl *bodyA{}, *bodyB{};

  // world-space location of contact point
  Vec2 pos;

  // contact normal, pointing A->B
  Vec2 normal;

  // penetration depth (B penetrating A, always positive)
  float depth;
};

} // namespace fysics::detail

namespace fysics {

// detect a collision between two shapes placed in world space by their
// respective body-to-world transforms. returns std::nullopt when the shapes do
// not overlap. the returned Contact has null bodyA/bodyB pointers; its normal
// points from shape a towards shape b.
std::optional<detail::Contact> shape_detect_collision(
    const Shape &a, const Mat23 &xf_a, const Shape &b, const Mat23 &xf_b
);

} // namespace fysics

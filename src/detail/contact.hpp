#pragma once

#include "body.hpp"

namespace fysics::detail {

struct Contact {
  Body *bodyA{}, *bodyB{};

  // world-space location of contact point
  Vec2 pos;

  // contact normal, pointing A->B
  Vec2 normal;
};

} // namespace fysics::detail

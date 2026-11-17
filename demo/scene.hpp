#pragma once

#include <vector>

#include "fysics/fysics.hpp"

namespace demo {

struct Scene {
  fysics::World world;
  std::vector<fysics::BodyId> bodies;
};

Scene make_two_circle_scene();

} // namespace demo

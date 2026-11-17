#include "scene.hpp"

namespace demo {

Scene make_two_circle_scene() {
  Scene scene{fysics::World{fysics::WorldConfig{{0.0f, 0.0f}}}, {}};
  fysics::BodyConfig circle;
  circle.shape = fysics::CircleShape{1.0f};
  circle.pos = {-2.0f, 0.0f};
  circle.vel = {1.0f, 0.0f};
  scene.bodies.push_back(scene.world.create_body(circle).id());

  circle.pos = {2.0f, 0.0f};
  circle.vel = {-1.0f, 0.0f};
  scene.bodies.push_back(scene.world.create_body(circle).id());
  return scene;
}

} // namespace demo

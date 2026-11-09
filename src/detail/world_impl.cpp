#include "world_impl.hpp"

#include "body.hpp"

using fysics::detail::BodyImpl;

namespace fysics {

bool World::Impl::is_id_valid(WorldId world_id) const {
  // an id of 0 means invalid
  return id != 0 && world_id == id;
}

void World::Impl::integrate(float dt) {
  for (auto &body : body_pool) {
    Vec2 acc = gravity;
    Vec2 vel = body.get_vel() + acc * dt;
    Vec2 pos = body.get_pos() + vel * dt;

    float ang_acc = 0;
    float ang_vel = body.get_ang_vel() + ang_acc * dt;
    float angle = body.get_angle() + ang_vel * dt;

    body.set_pos(pos);
    body.set_vel(vel);
    body.set_angle(angle);
    body.set_ang_vel(ang_vel);
  }
}

std::vector<detail::Contact> World::Impl::detect_collisions() {
  // TODO
  return std::vector<detail::Contact>{};
}

void World::Impl::resolve_collisions(const std::vector<detail::Contact> contacts) {
  // TODO
}

} // namespace fysics

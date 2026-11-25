#include "world_impl.hpp"

#include "body.hpp"

using fysics::detail::BodyImpl;

namespace fysics {

bool World::Impl::is_id_valid(WorldId world_id) const {
  // an id of 0 means invalid
  return id != 0 && world_id == id;
}

void World::Impl::step(float dt) {
  time += dt;

  integrate(dt);
  auto contacts(detect_collisions());
  resolve_collisions(contacts);
}

void World::Impl::integrate(float dt) {
  for (auto &body : body_pool) {
    body.integrate(dt);
  }
}

} // namespace fysics

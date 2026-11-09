#include "world_impl.hpp"

namespace fysics {

bool World::Impl::is_id_valid(WorldId world_id) const {
  // an id of 0 means invalid
  return id != 0 && world_id == id;
}

void World::Impl::integrate(float dt) {}

std::vector<detail::Contact> World::Impl::detect_collisions() {
  return std::vector<detail::Contact>{};
}

void World::Impl::resolve_collisions(const std::vector<detail::Contact> contacts) {}

} // namespace fysics

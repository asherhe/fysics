#include <memory>

#include "fysics/fysics.hpp"

#include "detail/body.hpp"
#include "detail/world_impl.hpp"

namespace fysics {

World::World(const WorldConfig &config) : impl_(std::make_unique<Impl>(config)) {}

Body World::create_body(const BodyConfig &config) {
  auto pool_id = impl_->body_pool.add(detail::Body{config});
  return Body(BodyId{pool_id.id, pool_id.generation, impl_->id}, *this);
}

Body World::get_body(BodyId id) {
  return Body(id, *this);
}

void World::delete_body(BodyId id) {
  impl_->body_pool.erase({id.id, id.generation});
}

void World::step(float dt) {
  impl_->integrate(dt);
  auto contacts = impl_->detect_collisions();
  impl_->resolve_collisions(contacts);
}

} // namespace fysics

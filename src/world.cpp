#include <memory>
#include "fysics/fysics.hpp"
#include "fysics/math.hpp"

#include "body.hpp"
#include "pool.hpp"

namespace fysics {

struct World::Impl {
  // global counter for distinct world ids
  inline static WorldId id_counter = 0;

  // unique id for world instance
  WorldId id = id_counter++;
  Vec2 gravity;
  
  // object pool of fysics::detail::Body rigidbody instances
  detail::Pool<detail::Body> body_pool;

  Impl(const WorldConfig &config) : gravity{config.gravity} {}
};

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

} // namespace fysics

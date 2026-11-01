#pragma once

#include "fysics/fysics.hpp"
#include "fysics/math.hpp"

#include "body.hpp"
#include "pool.hpp"

namespace fysics {

struct detail::WorldImplAccess {
  static World::Impl &get_impl(World &world) { return *world.impl_; }
};

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

} // namespace fysics

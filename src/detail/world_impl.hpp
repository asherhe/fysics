#pragma once

#include "fysics/fysics.hpp"
#include "fysics/math.hpp"

#include "body.hpp"
#include "contact.hpp"
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

  // object pool of fysics::detail::BodyImpl rigidbody instances
  detail::Pool<detail::BodyImpl> body_pool;

  Impl(const WorldConfig &config) : gravity{config.gravity} {}

  // advance simulation of velocities, accelerations
  void integrate(float dt);
  // determine pairs of fysics::Bodies that are in physical contact
  std::vector<detail::Contact> detect_collisions();
  // resolve contacts between fysics::Bodies by modifying their motion
  void resolve_collisions(std::vector<detail::Contact> contacts);
};

} // namespace fysics

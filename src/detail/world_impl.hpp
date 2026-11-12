#pragma once

#include "fysics/fysics.hpp"
#include "fysics/math.hpp"
#include "fysics/types.hpp"

#include "body.hpp"
#include "contact.hpp"
#include "pool.hpp"

namespace fysics {

struct detail::WorldImplAccess {
  static World::Impl &get_impl(World &world) { return *world.impl_; }
};

struct World::Impl {
  // global counter for distinct world ids
  // id 0 is reserved for invalid values
  inline static WorldId id_counter = 1;

  // unique id for world instance
  WorldId id = id_counter++;
  Vec2 gravity;

  float time = 0.0f;

  // object pool of fysics::detail::BodyImpl rigidbody instances
  detail::Pool<detail::BodyImpl> body_pool;

  Impl(const WorldConfig &config) : gravity{config.gravity} {}

  // whether a given WorldId matches the current world
  bool is_id_valid(WorldId world_id) const;

  void step(float dt);
  // advance simulation of velocities, accelerations
  void integrate(float dt);
  // determine pairs of fysics::Bodies that are in physical contact
  std::vector<detail::Contact> detect_collisions();
  // resolve contacts between fysics::Bodies by modifying their motion
  void resolve_collisions(const std::vector<detail::Contact> contacts);
};

} // namespace fysics

#pragma once

#include <memory>

#include "fysics/types.hpp"

namespace fysics {

class World;

// a rigidbody object simulated by the physics world
class Body {
private:
  // internal reference to a body under fysics::World.
  BodyId id_;
  World *world_;

  // only fysics::World can construct a new Body
  friend class World;
  explicit Body(BodyId id, World &world) noexcept : id_{id}, world_{&world} {}

public:
  [[nodiscard]] BodyId id() const noexcept { return id_; }

  Vec2 get_pos() const;
  void set_pos(const Vec2 &pos);

  Vec2 get_vel() const;
  void set_vel(const Vec2 &vel);

  float get_angle() const;
  void set_angle(float angle);

  float get_ang_vel() const;
  void set_ang_vel(float ang_vel);

  float get_mass() const;
  void set_mass(float mass);
  float get_inv_mass() const;

  float get_inertia() const;
  float get_inv_inertia() const;

  SimulationType get_sim_type() const;
  void set_sim_type(SimulationType sim_type);

  Shape get_shape() const;
  void set_shape(const Shape &shape);

  Vec2 get_center_of_mass() const;
  AABB get_world_aabb() const;
};

namespace detail {
// internal bridge to enable access to World
struct WorldImplAccess;
} // namespace detail

// physics world that manages simulation of all physics objects
class World {
private:
  // internal implementation of fysics::World - not publicly exposed
  struct Impl;
  std::unique_ptr<Impl> impl_;

  friend struct detail::WorldImplAccess;

public:
  explicit World(const WorldConfig &config);

  World(const World &world) = delete;
  World &operator=(const World &world) = delete;

  World(World &&world) noexcept;
  World &operator=(World &&world) noexcept;
  ~World();

  // instantiate a new fysics::Body in the physics world
  Body create_body(const BodyConfig &config);
  Body get_body(BodyId id);
  void delete_body(BodyId id);

  // advance physics simulation by dt seconds
  void step(float dt);
};

} // namespace fysics

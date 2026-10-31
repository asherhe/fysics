#pragma once

#include "fysics/types.hpp"
#include <memory>

namespace fysics {

class World {
private:
  // internal implementation of fysics::World - not publicly exposed
  struct Impl;
  std::unique_ptr<Impl> _impl;

  World(const WorldConfig &config) {} // todo: initialize fields

public:
  static WorldId &create(const WorldConfig &config);

  BodyId create_body(const BodyConfig &config);
  void delete_body(BodyId id);

  // getters, setters for Body fields 

  Vec2 get_body_pos(BodyId id);
  void set_body_pos(BodyId id, const Vec2 &pos);

  Vec2 get_body_vel(BodyId id);
  void set_body_vel(BodyId id, const Vec2 &vel);

  float get_body_angle(BodyId id);
  void set_body_angle(BodyId id, float angle);

  float get_body_ang_vel(BodyId id);
  void set_body_ang_vel(BodyId id, float ang_vel);

  float get_body_mass(BodyId id);
  void set_body_mass(BodyId id, float mass);

  SimulationType get_body_sim_type(BodyId id);
  void set_body_sim_type(BodyId id, SimulationType sim_type);

  void step(float dt);
};

} // namespace fysics

#pragma once

#include "fysics/math.hpp"
#include "fysics/types.hpp"

namespace fysics {

class Body {
private:
  Vec2 _pos, _vel;
  float _angle, _ang_vel;
  float _mass, _inv_mass;
  SimulationType _sim_type;
  Mat23 _body2world, _world2body;

  void update_derived_fields() noexcept;

public:
  explicit Body(const BodyConfig &config)
      : _pos{config.pos}, _vel{config.vel}, _angle{config.angle},
        _ang_vel{config.ang_vel}, _mass{config.mass},
        _sim_type{config.sim_type} {
    update_derived_fields();
  }

  Vec2 get_pos() const noexcept { return _pos; }
  void set_pos(const Vec2 &pos) noexcept {
    _pos = pos;
    update_derived_fields();
  }

  Vec2 get_vel() const noexcept { return _vel; }
  void set_vel(const Vec2 &vel) noexcept {
    _vel = vel;
    update_derived_fields();
  }

  float get_angle() const noexcept { return _angle; }
  void set_angle(float angle) noexcept {
    _angle = angle;
    update_derived_fields();
  }

  float get_ang_vel() const noexcept { return _ang_vel; }
  void set_ang_vel(float ang_vel) noexcept {
    _ang_vel = ang_vel;
    update_derived_fields();
  }

  float get_mass() const noexcept { return _mass; }
  void set_mass(float mass) noexcept {
    _mass = mass;
    update_derived_fields();
  }

  SimulationType get_sim_type() const noexcept { return _sim_type; }
  void set_sim_type(SimulationType sim_type) noexcept {
    _sim_type = sim_type;
    update_derived_fields();
  }
};

}

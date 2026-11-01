#pragma once

#include "fysics/math.hpp"
#include "fysics/types.hpp"

namespace fysics::detail {

// internal implementation of rigidbodies
// since the lifecycle of bodies is controlled by the fysics::World, the user
// should not have access to the underlying Body instance. instead, access is
// provided with the fysics::Body wrapper that points to a fysics::detail::Body
// instance in the World rigidbody pool.
class Body {
private:
  Vec2 pos_, vel_;
  float angle_, ang_vel_;
  Mat23 body2world_, world2body_;
  float mass_, inv_mass_;
  SimulationType sim_type_;
  Shape shape_;

  void update_derived_fields() noexcept;

public:
  explicit Body(const BodyConfig &config)
      : pos_{config.pos}, vel_{config.vel}, angle_{config.angle}, ang_vel_{config.ang_vel},
        mass_{config.mass}, sim_type_{config.sim_type}, shape_{config.shape} {
    update_derived_fields();
  }

  Vec2 get_pos() const noexcept { return pos_; }
  void set_pos(const Vec2 &pos) noexcept {
    pos_ = pos;
    update_derived_fields();
  }

  Vec2 get_vel() const noexcept { return vel_; }
  void set_vel(const Vec2 &vel) noexcept {
    vel_ = vel;
    update_derived_fields();
  }

  float get_angle() const noexcept { return angle_; }
  void set_angle(float angle) noexcept {
    angle_ = angle;
    update_derived_fields();
  }

  float get_ang_vel() const noexcept { return ang_vel_; }
  void set_ang_vel(float ang_vel) noexcept {
    ang_vel_ = ang_vel;
    update_derived_fields();
  }

  float get_mass() const noexcept { return mass_; }
  void set_mass(float mass) noexcept {
    mass_ = mass;
    update_derived_fields();
  }

  SimulationType get_sim_type() const noexcept { return sim_type_; }
  void set_sim_type(SimulationType sim_type) noexcept {
    sim_type_ = sim_type;
    update_derived_fields();
  }

  Shape get_shape() const noexcept { return shape_; }
  void set_shape(const Shape &shape) noexcept {
    shape_ = shape;
    update_derived_fields();
  }
};

} // namespace fysics::detail

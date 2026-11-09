#include "body.hpp"

#include "fysics/math.hpp"
#include "fysics/types.hpp"

namespace fysics::detail {

void BodyImpl::update_derived_fields() noexcept {
  if (sim_type_ == SimulationType::Static) {
    mass_ = inv_mass_ = 0.0f;
    inertia_ = inv_inertia_ = 0.0f;
  } else {
    inv_mass_ = 1.0f / mass_;
    inertia_ = mass_ * shape_moment_of_inertia(shape_);
    inv_inertia_ = 1.0f / inertia_;
  }
  center_of_mass_ = shape_center_of_mass(shape_);

  Mat23 trans_com = Mat23({}, -center_of_mass_);        // shift COM to origin
  Mat23 local_2_world = Mat23::rot_trans(angle_, pos_); // body pos and angle
  body2world_ = local_2_world * trans_com;
  world2body_ = body2world_.inverse_rot();

  world_aabb_ = shape_aabb(shape_, body2world_);
}

} // namespace fysics::detail

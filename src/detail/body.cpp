#include "body.hpp"

#include <variant>

#include "fysics/math.hpp"

namespace fysics::detail {

void BodyImpl::update_derived_fields() noexcept {
  inv_mass_ = 1.0f / mass_;
  center_of_mass_ = std::visit([](auto &&shape) { return shape.center_of_mass(); }, shape_);
  Mat23 trans_com = Mat23({}, -center_of_mass_); // shift COM to origin
  Mat23 local_2_world = Mat23::rot_trans(angle_, pos_); // body pos and angle
  body2world_ = local_2_world * trans_com;
  world2body_ = body2world_.inverse_rot();
}

} // namespace fysics::detail

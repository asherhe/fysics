#include "detail/body.hpp"

#include "fysics/fysics.hpp"
#include "fysics/types.hpp"

#include "detail/world_impl.hpp"

namespace {

using fysics::BodyId;
using fysics::World;
using fysics::detail::Body;
using fysics::detail::WorldImplAccess;

Body get_body(BodyId id, World *world) {
  return WorldImplAccess::get_impl(*world).body_pool.at({id.id, id.generation});
}

} // namespace

namespace fysics {

Vec2 Body::get_pos() const {
  return get_body(id_, world_).get_pos();
}
void Body::set_pos(const Vec2 &pos) {
  get_body(id_, world_).set_pos(pos);
}

Vec2 Body::get_vel() const {
  return get_body(id_, world_).get_vel();
}
void Body::set_vel(const Vec2 &vel) {
  get_body(id_, world_).set_vel(vel);
}

float Body::get_angle() const {
  return get_body(id_, world_).get_angle();
}
void Body::set_angle(float angle) {
  get_body(id_, world_).set_angle(angle);
}

float Body::get_ang_vel() const {
  return get_body(id_, world_).get_ang_vel();
}
void Body::set_ang_vel(float ang_vel) {
  get_body(id_, world_).set_ang_vel(ang_vel);
}

float Body::get_mass() const {
  return get_body(id_, world_).get_mass();
}
void Body::set_mass(float mass) {
  get_body(id_, world_).set_mass(mass);
}

SimulationType Body::get_sim_type() const {
  return get_body(id_, world_).get_sim_type();
}
void Body::set_sim_type(SimulationType sim_type) {
  get_body(id_, world_).set_sim_type(sim_type);
}

Shape Body::get_shape() const {
  return get_body(id_, world_).get_shape();
}
void Body::set_shape(const Shape &shape) {
  get_body(id_, world_).set_shape(shape);
}

} // namespace fysics

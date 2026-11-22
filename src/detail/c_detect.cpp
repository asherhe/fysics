#include <optional>
#include <algorithm>

#include "fysics/math.hpp"
#include "world_impl.hpp"

using std::clamp;

namespace fysics {

std::optional<detail::Contact> collide(
  const CircleShape& a, Mat23 xf_a, 
  const CircleShape& b, Mat23 xf_b
) {
  Vec2 a_center = xf_a.transform_point(a.center_of_mass());
  Vec2 b_center = xf_b.transform_point(b.center_of_mass());
  Vec2 d = b_center - a_center;
  float dist = d.norm();
  if (dist >= a.radius + b.radius) return std::nullopt;

  Vec2 n = d / dist;
  return detail::Contact{nullptr, nullptr, a_center + n * a.radius, n};
}

std::optional<detail::Contact> collide(
  const CircleShape& a, Mat23 xf_a, 
  const BoxShape& b, Mat23 xf_b
) {
  Mat23 b_from_a = xf_b.inverse_rot() * xf_a;    // A body coords -> B
  Vec2 ca_b = b_from_a.transform_point(a.center_of_mass());
  
  float half_x = b.size.x / 2, half_y = b.size.y / 2;
  Vec2 clamped{clamp(ca_b.x, -half_x, half_x), clamp(ca_b.y, -half_y, half_y)};
  Vec2 d = clamped - ca_b;
  float dist = d.norm();
  if (dist >= a.radius) return std::nullopt;

  // TODO: guard against dist == 0
  Vec2 n = d / dist;
  Vec2 contact_b = ca_b + n * a.radius;     // in B coords
  Vec2 contact = xf_b.transform_point(contact_b);
  return detail::Contact{nullptr, nullptr, contact, xf_b.transform_vector(n)};
}

std::optional<detail::Contact> collide(
  const BoxShape& a, Mat23 xf_a, 
  const CircleShape& b, Mat23 xf_b
) {
  auto c = collide(b, xf_b, a, xf_a);
  if (c) c->normal = -c->normal;            // normal must be A->B in original 
  return c;
}

std::vector<detail::Contact> World::Impl::detect_collisions() {
  // TODO
  return std::vector<detail::Contact>{};
}

} // namespace fysics

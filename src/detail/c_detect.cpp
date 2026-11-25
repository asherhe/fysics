#include <optional>
#include <algorithm>
#include <variant>

#include "fysics/math.hpp"
#include "world_impl.hpp"

using std::clamp;
using std::abs;

namespace fysics {

// projection half-extents of a box onto a line
float proj_radius(const BoxShape& box, const Mat23& xf, const Vec2& L) {
  Vec2 half_x = xf.linear.c0 * (0.5f * box.size.x);
  Vec2 half_y = xf.linear.c1 * (0.5f * box.size.y);
  return abs(half_x.dot(L)) + abs(half_y.dot(L));
}

std::optional<detail::Contact> collide(
  const CircleShape& a, const Mat23& xf_a, 
  const CircleShape& b, const Mat23& xf_b
) {
  Vec2 a_center = xf_a.transform_point(a.center_of_mass());
  Vec2 b_center = xf_b.transform_point(b.center_of_mass());
  Vec2 d = b_center - a_center;
  float dist = d.norm();
  if (dist >= a.radius + b.radius) return std::nullopt;

  Vec2 n = d / dist;
  float penet_depth = a.radius + b.radius - dist;
  return detail::Contact{nullptr, nullptr, a_center + n * a.radius, n, penet_depth};
}

std::optional<detail::Contact> collide(
  const CircleShape& a, const Mat23& xf_a, 
  const BoxShape& b, const Mat23& xf_b
) {
  Mat23 b_from_a = xf_b.inverse_rot() * xf_a;    // A body coords -> B
  Vec2 ca_b = b_from_a.transform_point(a.center_of_mass());
  
  float half_x = b.size.x / 2, half_y = b.size.y / 2;
  Vec2 clamped{clamp(ca_b.x, -half_x, half_x), clamp(ca_b.y, -half_y, half_y)};
  Vec2 d = clamped - ca_b;
  float dist = d.norm();
  if (dist >= a.radius) return std::nullopt;

  Vec2 n, contact;
  float penet_depth;
  // circle center is inside box
  if (dist == 0.0f) {
    // distance to nearest faces
    float dx = half_x - abs(ca_b.x);
    float dy = half_y - abs(ca_b.y);
    
    if (dx < dy) {
      n = { ca_b.x < 0.0f ? -1.0f : 1.0f, 0.0f };
      penet_depth = a.radius + dx;
    } else {
      n = { 0.0f, ca_b.y < 0.0f ? -1.0f : 1.0f };
      penet_depth = a.radius + dy;
    }
    contact = xf_b.transform_point(ca_b);
  } else {
    n = d / dist;
    Vec2 contact_b = ca_b + n * a.radius;     // in B coords
    contact = xf_b.transform_point(contact_b);
    penet_depth = a.radius - dist;
  }

  return detail::Contact{nullptr, nullptr, contact, xf_b.transform_vector(n), penet_depth};
}

std::optional<detail::Contact> collide(
  const BoxShape& a, const Mat23& xf_a, 
  const CircleShape& b, const Mat23& xf_b
) {
  auto c = collide(b, xf_b, a, xf_a);
  if (c) c->normal = -c->normal;            // normal must be A->B in original 
  return c;
}

std::optional<detail::Contact> collide(
  const BoxShape& a, const Mat23& xf_a, 
  const BoxShape& b, const Mat23& xf_b 
) {
  Vec2 axes[4] = { xf_a.linear.c0, xf_a.linear.c1, xf_b.linear.c0, xf_b.linear.c1 };
  Vec2 ca = xf_a.transform_point(a.center_of_mass());
  Vec2 cb = xf_b.transform_point(b.center_of_mass());
  Vec2 c_dist = cb - ca;

  float max_sep = -std::numeric_limits<float>::infinity();
  Vec2 sep_axis;
  int penet_box_idx = 0;
  for (std::size_t i = 0; i < 4; ++i) {
    Vec2& L = axes[i];
    float sep = abs(c_dist.dot(L)) - (proj_radius(a, xf_a, L) + proj_radius(b, xf_b, L));
    if (sep >= 0.0f) return std::nullopt;
    if (sep > max_sep) { 
      max_sep = sep; 
      sep_axis = L; 
      penet_box_idx = (i < 2) ? 1 : 0;
    }
  }
  Vec2 n = sep_axis * (c_dist.dot(sep_axis) < 0.0f ? -1.0f : 1.0f);

  const BoxShape& penet_box = penet_box_idx ? b : a;
  const Mat23& penet_xf = penet_box_idx ? xf_b : xf_a;
  const BoxShape& recev_box = penet_box_idx ? a : b;
  const Mat23& recev_xf = penet_box_idx ? xf_a : xf_b;

  const Vec2 corner_distances[4] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  float min_dist = std::numeric_limits<float>::infinity();
  Vec2 contact;
  for (const Vec2& v : corner_distances) {
    Vec2 d_from_ctr = {0.5f * penet_box.size.x * v.x, 0.5f * penet_box.size.y * v.y};
    Vec2 corner_in_world = penet_xf.transform_point(d_from_ctr);
    float dist = (recev_xf.transform_point(recev_box.center_of_mass()) - corner_in_world).norm();
    if (dist < min_dist) { 
      min_dist = dist;
      contact = corner_in_world; 
    }
  }

  return detail::Contact{nullptr, nullptr, contact, n, -max_sep};
}

std::optional<detail::Contact> shape_detect_collision(
  const Shape& a, const Mat23& xf_a, 
  const Shape& b, const Mat23& xf_b
) {
  return std::visit([&](auto& sa, auto& sb) { return collide(sa, xf_a, sb, xf_b); }, a, b);
}

std::vector<detail::Contact> World::Impl::detect_collisions() {
  std::vector<detail::Contact> contacts;

  for (auto a = body_pool.begin(); a != body_pool.end(); ++a) {
    auto b = a;
    for (++b; b != body_pool.end(); ++b) {
      detail::BodyImpl& body_a = *a;
      detail::BodyImpl& body_b = *b;
      auto contact = shape_detect_collision(
        body_a.get_shape(), body_a.get_body2world(), 
        body_b.get_shape(), body_b.get_body2world()
      );
      // both bodies cannot move
      if (body_a.get_sim_type() == SimulationType::Static && 
          body_b.get_sim_type() == SimulationType::Static)
        continue;

      // contact BodyImpls are filled in here
      if (contact) {
        contact->bodyA = &body_a;
        contact->bodyB = &body_b;
        contacts.push_back(*contact);
      }
    }
  }
  return contacts;
}

} // namespace fysics

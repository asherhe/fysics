#pragma once

#include <cmath>

namespace fysics {
struct Vec2 {
  float x, y;

  constexpr Vec2() : x(0.0f), y(0.0f) {}
  constexpr Vec2(float x, float y) : x(x), y(y) {}

  constexpr Vec2 operator-() const { return {-x, -y}; }

  constexpr Vec2 &operator+=(const Vec2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  constexpr Vec2 &operator-=(const Vec2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  constexpr Vec2 &operator*=(float s) {
    x *= s;
    y *= s;
    return *this;
  }
  constexpr Vec2 &operator/=(float s) {
    x /= s;
    y /= s;
    return *this;
  }

  constexpr float dot(const Vec2 &v) const { return x * v.x + y * v.y; }

  // z-component of (x, y, 0) x (v.x, v.y, 0)
  constexpr float cross(const Vec2 &v) const { return x * v.y - y * v.x; }

  // (0, 0, s) x (x, y, 0)
  static constexpr Vec2 cross(float s, const Vec2 &v) {
    return {-s * v.y, s * v.x};
  }
  // (x, y, 0) x (0, 0, s)
  constexpr Vec2 cross(float s) const { return {s * y, -s * x}; }

  // counterclockwise perpendicular, equivalent to cross(1, *this)
  constexpr Vec2 perp() const { return {-y, x}; }

  constexpr float norm2() const { return dot(*this); }
  float norm() const { return std::sqrt(norm2()); }

  // returns the unit vector in this direction, or the zero vector if this has
  // zero length
  Vec2 normalize() const {
    float n = norm();
    if (n == 0.0f)
      return {};
    return {x / n, y / n};
  }
};

constexpr Vec2 operator+(Vec2 a, const Vec2 &b) { return a += b; }
constexpr Vec2 operator-(Vec2 a, const Vec2 &b) { return a -= b; }
constexpr Vec2 operator*(Vec2 v, float s) { return v *= s; }
constexpr Vec2 operator*(float s, Vec2 v) { return v *= s; }
constexpr Vec2 operator/(Vec2 v, float s) { return v /= s; }

// 2x2 matrix stored as columns: [c0 c1]
struct Mat22 {
  Vec2 c0, c1;

  // identity
  constexpr Mat22() : c0(1.0f, 0.0f), c1(0.0f, 1.0f) {}
  constexpr Mat22(const Vec2 &c0, const Vec2 &c1) : c0(c0), c1(c1) {}
  // row-major element order
  constexpr Mat22(float m00, float m01, float m10, float m11)
      : c0(m00, m10), c1(m01, m11) {}

  static constexpr Mat22 identity() { return {}; }
  static constexpr Mat22 zero() { return {Vec2{}, Vec2{}}; }

  // counterclockwise rotation by angle radians
  static Mat22 rotation(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    return {c, -s, s, c};
  }

  constexpr Mat22 &operator+=(const Mat22 &m) {
    c0 += m.c0;
    c1 += m.c1;
    return *this;
  }
  constexpr Mat22 &operator-=(const Mat22 &m) {
    c0 -= m.c0;
    c1 -= m.c1;
    return *this;
  }
  constexpr Mat22 &operator*=(float s) {
    c0 *= s;
    c1 *= s;
    return *this;
  }

  constexpr Mat22 transpose() const {
    return {Vec2{c0.x, c1.x}, Vec2{c0.y, c1.y}};
  }

  constexpr float determinant() const { return c0.cross(c1); }

  // returns the zero matrix if this is singular. for a pure rotation, prefer
  // transpose(), which is the same thing and cheaper
  constexpr Mat22 inverse() const {
    float det = determinant();
    if (det == 0.0f)
      return zero();
    float inv = 1.0f / det;
    return {c1.y * inv, -c1.x * inv, -c0.y * inv, c0.x * inv};
  }

  // solves this * x = b for x, without forming the inverse. returns the zero
  // vector if this is singular
  constexpr Vec2 solve(const Vec2 &b) const {
    float det = determinant();
    if (det == 0.0f)
      return {};
    float inv = 1.0f / det;
    return {inv * b.cross(c1), inv * c0.cross(b)};
  }
};

constexpr Mat22 operator+(Mat22 a, const Mat22 &b) { return a += b; }
constexpr Mat22 operator-(Mat22 a, const Mat22 &b) { return a -= b; }
constexpr Mat22 operator*(Mat22 m, float s) { return m *= s; }
constexpr Mat22 operator*(float s, Mat22 m) { return m *= s; }

constexpr Vec2 operator*(const Mat22 &m, const Vec2 &v) {
  return m.c0 * v.x + m.c1 * v.y;
}
constexpr Mat22 operator*(const Mat22 &a, const Mat22 &b) {
  return {a * b.c0, a * b.c1};
}

// 2D affine transform: the top two rows of a 3x3 homogeneous matrix
//   [ linear  translation ]
//   [ 0  0    1           ]  <- implicit, never stored
struct Mat23 {
  Mat22 linear;
  Vec2 translation;

  // identity
  constexpr Mat23() = default;
  constexpr Mat23(const Mat22 &linear, const Vec2 &translation)
      : linear(linear), translation(translation) {}

  static constexpr Mat23 identity() { return {}; }

  // rotate counterclockwise by angle radians, then translate
  static Mat23 rot_trans(float angle, const Vec2 &translation) {
    return {Mat22::rotation(angle), translation};
  }

  // homogeneous w = 1: affected by translation
  constexpr Vec2 transform_point(const Vec2 &p) const {
    return linear * p + translation;
  }
  // homogeneous w = 0: unaffected by translation
  constexpr Vec2 transform_vector(const Vec2 &v) const { return linear * v; }

  // general affine inverse. returns a zero linear part if this is singular
  constexpr Mat23 inverse() const {
    Mat22 inv = linear.inverse();
    return {inv, -(inv * translation)};
  }

  // inverse assuming linear is a pure rotation
  constexpr Mat23 inverse_rot() const {
    Mat22 inv = linear.transpose();
    return {inv, -(inv * translation)};
  }
};

// composition: (a * b) applies b first, then a
constexpr Mat23 operator*(const Mat23 &a, const Mat23 &b) {
  return {a.linear * b.linear, a.linear * b.translation + a.translation};
}
} // namespace fysics

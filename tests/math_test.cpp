#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "fysics/math.hpp"

using Catch::Matchers::WithinAbs;
using fysics::Mat22;
using fysics::Mat23;
using fysics::Vec2;

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float EPS = 1e-5f;

// exact comparisons, usable in STATIC_REQUIRE. test values are chosen to be
// exactly representable so these don't depend on rounding
constexpr bool equal(const Vec2 &a, const Vec2 &b) {
  return a.x == b.x && a.y == b.y;
}
constexpr bool equal(const Mat22 &a, const Mat22 &b) {
  return equal(a.c0, b.c0) && equal(a.c1, b.c1);
}

// approximate comparisons for results that go through sqrt/sin/cos
void require_near(const Vec2 &actual, const Vec2 &expected) {
  REQUIRE_THAT(actual.x, WithinAbs(expected.x, EPS));
  REQUIRE_THAT(actual.y, WithinAbs(expected.y, EPS));
}
void require_near(const Mat22 &actual, const Mat22 &expected) {
  require_near(actual.c0, expected.c0);
  require_near(actual.c1, expected.c1);
}

} // namespace


// ---- Vec2 ----

TEST_CASE("Vec2 construction", "[math][vec2]") {
  STATIC_REQUIRE(equal(Vec2{}, Vec2{0.0f, 0.0f}));

  constexpr Vec2 v{1.0f, -2.0f};
  STATIC_REQUIRE(v.x == 1.0f);
  STATIC_REQUIRE(v.y == -2.0f);
}

TEST_CASE("Vec2 arithmetic operators", "[math][vec2]") {
  constexpr Vec2 a{1.0f, 2.0f};
  constexpr Vec2 b{3.0f, 4.0f};

  STATIC_REQUIRE(equal(-a, Vec2{-1.0f, -2.0f}));
  STATIC_REQUIRE(equal(a + b, Vec2{4.0f, 6.0f}));
  STATIC_REQUIRE(equal(a - b, Vec2{-2.0f, -2.0f}));
  STATIC_REQUIRE(equal(a * 2.0f, Vec2{2.0f, 4.0f}));
  STATIC_REQUIRE(equal(2.0f * a, Vec2{2.0f, 4.0f}));
  STATIC_REQUIRE(equal(b / 2.0f, Vec2{1.5f, 2.0f}));
}

TEST_CASE("Vec2 compound assignment modifies in place", "[math][vec2]") {
  Vec2 v{1.0f, 2.0f};

  v += Vec2{3.0f, 4.0f};
  REQUIRE(equal(v, Vec2{4.0f, 6.0f}));

  v -= Vec2{1.0f, 1.0f};
  REQUIRE(equal(v, Vec2{3.0f, 5.0f}));

  v *= 2.0f;
  REQUIRE(equal(v, Vec2{6.0f, 10.0f}));

  v /= 4.0f;
  REQUIRE(equal(v, Vec2{1.5f, 2.5f}));
}

TEST_CASE("Vec2 dot product", "[math][vec2]") {
  STATIC_REQUIRE(Vec2{1.0f, 2.0f}.dot(Vec2{3.0f, 4.0f}) == 11.0f);
  STATIC_REQUIRE(Vec2{1.0f, 0.0f}.dot(Vec2{0.0f, 1.0f}) == 0.0f);
}

TEST_CASE("Vec2 cross products", "[math][vec2]") {
  SECTION("vector x vector gives the z component") {
    STATIC_REQUIRE(Vec2{1.0f, 0.0f}.cross(Vec2{0.0f, 1.0f}) == 1.0f);
    STATIC_REQUIRE(Vec2{0.0f, 1.0f}.cross(Vec2{1.0f, 0.0f}) == -1.0f);
    STATIC_REQUIRE(Vec2{2.0f, 3.0f}.cross(Vec2{2.0f, 3.0f}) == 0.0f);
  }

  SECTION("scalar x vector") {
    STATIC_REQUIRE(equal(Vec2::cross(2.0f, Vec2{1.0f, 3.0f}), Vec2{-6.0f, 2.0f}));
  }

  SECTION("vector x scalar") {
    STATIC_REQUIRE(equal(Vec2{1.0f, 3.0f}.cross(2.0f), Vec2{6.0f, -2.0f}));
  }

  SECTION("vector x scalar is the negation of scalar x vector") {
    constexpr Vec2 v{1.0f, 3.0f};
    STATIC_REQUIRE(equal(v.cross(2.0f), -Vec2::cross(2.0f, v)));
  }
}

TEST_CASE("Vec2 perp is the counterclockwise perpendicular", "[math][vec2]") {
  constexpr Vec2 v{1.0f, 2.0f};
  STATIC_REQUIRE(equal(v.perp(), Vec2{-2.0f, 1.0f}));
  STATIC_REQUIRE(equal(v.perp(), Vec2::cross(1.0f, v)));
  STATIC_REQUIRE(v.dot(v.perp()) == 0.0f);
}

TEST_CASE("Vec2 norm", "[math][vec2]") {
  constexpr Vec2 v{3.0f, 4.0f};
  STATIC_REQUIRE(v.norm2() == 25.0f);
  REQUIRE(v.norm() == 5.0f);
}

TEST_CASE("Vec2 normalize", "[math][vec2]") {
  SECTION("non-zero vector becomes unit length in the same direction") {
    const Vec2 n = Vec2{3.0f, 4.0f}.normalize();
    require_near(n, Vec2{0.6f, 0.8f});
    REQUIRE_THAT(n.norm(), WithinAbs(1.0f, EPS));
  }

  SECTION("zero vector stays zero") {
    REQUIRE(equal(Vec2{}.normalize(), Vec2{}));
  }
}


// ---- Mat22 ----

TEST_CASE("Mat22 construction", "[math][mat22]") {
  SECTION("default is identity") {
    constexpr Mat22 m;
    STATIC_REQUIRE(equal(m.c0, Vec2{1.0f, 0.0f}));
    STATIC_REQUIRE(equal(m.c1, Vec2{0.0f, 1.0f}));
    STATIC_REQUIRE(equal(m, Mat22::identity()));
  }

  SECTION("zero") {
    STATIC_REQUIRE(equal(Mat22::zero().c0, Vec2{}));
    STATIC_REQUIRE(equal(Mat22::zero().c1, Vec2{}));
  }

  SECTION("four floats are taken in row-major order") {
    // [1 2]
    // [3 4]
    constexpr Mat22 m{1.0f, 2.0f, 3.0f, 4.0f};
    STATIC_REQUIRE(equal(m.c0, Vec2{1.0f, 3.0f}));
    STATIC_REQUIRE(equal(m.c1, Vec2{2.0f, 4.0f}));
  }
}

TEST_CASE("Mat22 arithmetic operators", "[math][mat22]") {
  constexpr Mat22 a{1.0f, 2.0f, 3.0f, 4.0f};
  constexpr Mat22 b{5.0f, 6.0f, 7.0f, 8.0f};

  STATIC_REQUIRE(equal(a + b, Mat22{6.0f, 8.0f, 10.0f, 12.0f}));
  STATIC_REQUIRE(equal(b - a, Mat22{4.0f, 4.0f, 4.0f, 4.0f}));
  STATIC_REQUIRE(equal(a * 2.0f, Mat22{2.0f, 4.0f, 6.0f, 8.0f}));
  STATIC_REQUIRE(equal(2.0f * a, Mat22{2.0f, 4.0f, 6.0f, 8.0f}));
}

TEST_CASE("Mat22 multiplication", "[math][mat22]") {
  constexpr Mat22 a{1.0f, 2.0f, 3.0f, 4.0f};
  constexpr Mat22 b{5.0f, 6.0f, 7.0f, 8.0f};

  SECTION("matrix x vector") {
    STATIC_REQUIRE(equal(a * Vec2{5.0f, 6.0f}, Vec2{17.0f, 39.0f}));
  }

  SECTION("matrix x matrix") {
    STATIC_REQUIRE(equal(a * b, Mat22{19.0f, 22.0f, 43.0f, 50.0f}));
  }

  SECTION("identity is neutral") {
    STATIC_REQUIRE(equal(a * Mat22::identity(), a));
    STATIC_REQUIRE(equal(Mat22::identity() * a, a));
  }
}

TEST_CASE("Mat22 transpose and determinant", "[math][mat22]") {
  constexpr Mat22 m{1.0f, 2.0f, 3.0f, 4.0f};
  STATIC_REQUIRE(equal(m.transpose(), Mat22{1.0f, 3.0f, 2.0f, 4.0f}));
  STATIC_REQUIRE(m.determinant() == -2.0f);
  STATIC_REQUIRE(Mat22::identity().determinant() == 1.0f);
}

TEST_CASE("Mat22 inverse", "[math][mat22]") {
  SECTION("invertible matrix") {
    constexpr Mat22 m{1.0f, 2.0f, 3.0f, 4.0f};
    STATIC_REQUIRE(equal(m.inverse(), Mat22{-2.0f, 1.0f, 1.5f, -0.5f}));
    STATIC_REQUIRE(equal(m * m.inverse(), Mat22::identity()));
    STATIC_REQUIRE(equal(m.inverse() * m, Mat22::identity()));
  }

  SECTION("singular matrix gives zero") {
    constexpr Mat22 singular{1.0f, 2.0f, 2.0f, 4.0f};
    STATIC_REQUIRE(singular.determinant() == 0.0f);
    STATIC_REQUIRE(equal(singular.inverse(), Mat22::zero()));
  }
}

TEST_CASE("Mat22 solve", "[math][mat22]") {
  SECTION("invertible matrix") {
    constexpr Mat22 m{1.0f, 2.0f, 3.0f, 4.0f};
    STATIC_REQUIRE(equal(m.solve(Vec2{17.0f, 39.0f}), Vec2{5.0f, 6.0f}));
  }

  SECTION("singular matrix gives zero") {
    constexpr Mat22 singular{1.0f, 2.0f, 2.0f, 4.0f};
    STATIC_REQUIRE(equal(singular.solve(Vec2{1.0f, 1.0f}), Vec2{}));
  }
}

TEST_CASE("Mat22 rotation", "[math][mat22]") {
  SECTION("rotates counterclockwise") {
    const Mat22 r = Mat22::rotation(PI / 2.0f);
    require_near(r * Vec2{1.0f, 0.0f}, Vec2{0.0f, 1.0f});
    require_near(r * Vec2{0.0f, 1.0f}, Vec2{-1.0f, 0.0f});
  }

  SECTION("zero angle is identity") {
    require_near(Mat22::rotation(0.0f), Mat22::identity());
  }

  SECTION("transpose is the inverse") {
    const Mat22 r = Mat22::rotation(0.7f);
    require_near(r.transpose() * r, Mat22::identity());
    require_near(r.transpose(), r.inverse());
    REQUIRE_THAT(r.determinant(), WithinAbs(1.0f, EPS));
  }
}


// ---- Mat23 ----

TEST_CASE("Mat23 default is identity", "[math][mat23]") {
  constexpr Mat23 t;
  constexpr Vec2 p{3.0f, -2.0f};
  STATIC_REQUIRE(equal(t.linear, Mat22::identity()));
  STATIC_REQUIRE(equal(t.translation, Vec2{}));
  STATIC_REQUIRE(equal(t.transform_point(p), p));
  STATIC_REQUIRE(equal(Mat23::identity().transform_point(p), p));
}

TEST_CASE("Mat23 points are translated, vectors are not", "[math][mat23]") {
  constexpr Mat23 t{Mat22{2.0f, 0.0f, 0.0f, 4.0f}, Vec2{1.0f, -1.0f}};
  STATIC_REQUIRE(equal(t.transform_point(Vec2{1.0f, 1.0f}), Vec2{3.0f, 3.0f}));
  STATIC_REQUIRE(equal(t.transform_vector(Vec2{1.0f, 1.0f}), Vec2{2.0f, 4.0f}));
}

TEST_CASE("Mat23 rot_trans rotates then translates", "[math][mat23]") {
  const Mat23 t = Mat23::rot_trans(PI / 2.0f, Vec2{1.0f, 0.0f});
  require_near(t.transform_point(Vec2{1.0f, 0.0f}), Vec2{1.0f, 1.0f});
  require_near(t.transform_vector(Vec2{1.0f, 0.0f}), Vec2{0.0f, 1.0f});
}

TEST_CASE("Mat23 inverse", "[math][mat23]") {
  SECTION("general inverse undoes the transform") {
    constexpr Mat23 t{Mat22{2.0f, 0.0f, 0.0f, 4.0f}, Vec2{1.0f, -1.0f}};
    constexpr Vec2 p{1.0f, 1.0f};
    STATIC_REQUIRE(equal(t.inverse().transform_point(t.transform_point(p)), p));
    STATIC_REQUIRE(equal(t.transform_point(t.inverse().transform_point(p)), p));
  }

  SECTION("singular linear part gives a zero linear inverse") {
    constexpr Mat23 t{Mat22::zero(), Vec2{1.0f, 2.0f}};
    STATIC_REQUIRE(equal(t.inverse().linear, Mat22::zero()));
  }

  SECTION("inverse_rot undoes a rigid transform") {
    const Mat23 t = Mat23::rot_trans(0.7f, Vec2{3.0f, -2.0f});
    const Vec2 p{1.0f, 2.0f};
    require_near(t.inverse_rot().transform_point(t.transform_point(p)), p);
    require_near(t.inverse_rot().linear, t.inverse().linear);
    require_near(t.inverse_rot().translation, t.inverse().translation);
  }
}

TEST_CASE("Mat23 composition applies the right operand first", "[math][mat23]") {
  constexpr Mat23 translate{Mat22::identity(), Vec2{1.0f, 0.0f}};
  constexpr Mat23 scale{Mat22{2.0f, 0.0f, 0.0f, 2.0f}, Vec2{}};
  constexpr Vec2 p{1.0f, 1.0f};

  // scale, then translate
  STATIC_REQUIRE(equal((translate * scale).transform_point(p), Vec2{3.0f, 2.0f}));
  // translate, then scale
  STATIC_REQUIRE(equal((scale * translate).transform_point(p), Vec2{4.0f, 2.0f}));

  STATIC_REQUIRE(equal((translate * scale).transform_point(p),
                       translate.transform_point(scale.transform_point(p))));
}

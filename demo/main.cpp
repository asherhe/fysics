#include <iostream>

#include "fysics/fysics.hpp"
#include "fysics/types.hpp"

int main() {
  fysics::WorldConfig w_cfg{{0.0f, 0.0f}};
  fysics::World w{w_cfg};

  fysics::CircleShape circle{1.0f};

  fysics::BodyConfig b1_cfg{
      {-2.0f, 0.0f}, // position
      {1.0f, 0.0f},  // velocity
      0.0f,          // angle
      0.0f,          // angular velocity
      1.0f,          // mass
      fysics::SimulationType::Dynamic,
      circle,
  };
  fysics::Body b1 = w.create_body(b1_cfg);

  fysics::BodyConfig b2_cfg{
      {2.0f, 0.0f},  // position
      {-1.0f, 0.0f}, // velocity
      0.0f,          // angle
      0.0f,          // angular velocity
      1.0f,          // mass
      fysics::SimulationType::Dynamic,
      circle,
  };
  fysics::Body b2 = w.create_body(b2_cfg);

  for (int i = 0; i < 50; ++i) {
    std::cout << "t=" << w.get_time() << "; b1=" << b1.get_pos() << "; b2=" << b2.get_pos() << "\n";
    w.step(0.1);
  }

  return 0;
}

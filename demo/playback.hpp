#pragma once

#include <algorithm>

#include "scene.hpp"

namespace demo {

class Playback {
public:
  explicit Playback(Scene (*preset)()) : preset_{preset}, scene_{preset()} {}

  Scene &scene() { return scene_; }
  bool paused() const { return paused_; }

  void toggle_pause() {
    paused_ = !paused_;
    discard_elapsed();
  }

  void step_once() {
    if (paused_)
      scene_.world.step(static_cast<float>(step_seconds));
  }

  void reset() {
    scene_ = preset_();
    discard_elapsed();
  }

  void discard_elapsed() {
    pending_seconds_ = 0.0;
    discard_next_frame_ = true;
  }

  void advance(double elapsed_seconds) {
    if (discard_next_frame_) {
      discard_next_frame_ = false;
      return;
    }
    if (paused_)
      return;

    pending_seconds_ += std::min(elapsed_seconds, 0.05);
    while (pending_seconds_ >= step_seconds) {
      scene_.world.step(static_cast<float>(step_seconds));
      pending_seconds_ -= step_seconds;
    }
  }

private:
  static constexpr double step_seconds = 1.0 / 120.0;

  Scene (*preset_)();
  Scene scene_;
  double pending_seconds_ = 0.0;
  bool paused_ = false;
  bool discard_next_frame_ = true;
};

} // namespace demo

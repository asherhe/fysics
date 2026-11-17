#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <variant>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "playback.hpp"

namespace {

bool sdl_error(const char *operation) {
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s failed: %s", operation, SDL_GetError());
  return false;
}

class Frontend {
public:
  Frontend() = default;
  Frontend(const Frontend &) = delete;
  Frontend &operator=(const Frontend &) = delete;

  ~Frontend() {
    if (render_backend_)
      ImGui_ImplSDLRenderer3_Shutdown();
    if (platform_backend_)
      ImGui_ImplSDL3_Shutdown();
    if (imgui_)
      ImGui::DestroyContext(imgui_);
    if (renderer_)
      SDL_DestroyRenderer(renderer_);
    if (window_)
      SDL_DestroyWindow(window_);
    SDL_Quit();
  }

  bool init() {
    if (!SDL_Init(SDL_INIT_VIDEO))
      return sdl_error("SDL_Init");

    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    const float initial_scale = scale > 0.0f ? scale : 1.0f;
    if (!SDL_CreateWindowAndRenderer(
            "Fysics",
            static_cast<int>(960 * initial_scale),
            static_cast<int>(540 * initial_scale),
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY,
            &window_,
            &renderer_
        ))
      return sdl_error("SDL_CreateWindowAndRenderer");

    IMGUI_CHECKVERSION();
    imgui_ = ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    platform_backend_ = ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
    if (!platform_backend_) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize the ImGui SDL3 backend");
      return false;
    }
    render_backend_ = ImGui_ImplSDLRenderer3_Init(renderer_);
    if (!render_backend_) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize the ImGui SDL renderer");
      return false;
    }
    vsync_ = SDL_SetRenderVSync(renderer_, 1);
    return true;
  }

  int run(demo::Playback &playback) {
    const SDL_WindowID window_id = SDL_GetWindowID(window_);
    Uint64 previous_frame = SDL_GetTicksNS();
    for (;;) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window_id))
          return EXIT_SUCCESS;
        if ((event.type == SDL_EVENT_WINDOW_MINIMIZED || event.type == SDL_EVENT_WINDOW_RESTORED) &&
            event.window.windowID == window_id)
          playback.discard_elapsed();
      }

      const Uint64 frame_start = SDL_GetTicksNS();
      const double elapsed_seconds =
          static_cast<double>(frame_start - previous_frame) / SDL_NS_PER_SECOND;
      previous_frame = frame_start;
      if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) {
        playback.discard_elapsed();
        SDL_Delay(16);
        continue;
      }
      if (!render(playback, elapsed_seconds))
        return EXIT_FAILURE;

      constexpr Uint64 frame_duration = SDL_NS_PER_SECOND / 60;
      const Uint64 elapsed = SDL_GetTicksNS() - frame_start;
      if (!vsync_ && elapsed < frame_duration)
        SDL_DelayNS(frame_duration - elapsed);
    }
  }

private:
  void update_style(float scale) {
    if (scale == style_scale_)
      return;

    ImGuiStyle style;
    ImGui::StyleColorsDark(&style);
    style.WindowPadding = {12.0f, 8.0f};
    style.FramePadding = {10.0f, 5.0f};
    style.ItemSpacing = {8.0f, 6.0f};
    style.FrameRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = {0.10f, 0.09f, 0.17f, 1.0f};
    style.Colors[ImGuiCol_Border] = {0.24f, 0.22f, 0.35f, 1.0f};
    style.Colors[ImGuiCol_Text] = {0.94f, 0.93f, 0.98f, 1.0f};
    style.Colors[ImGuiCol_Button] = {0.20f, 0.18f, 0.30f, 1.0f};
    style.Colors[ImGuiCol_ButtonHovered] = {0.29f, 0.26f, 0.42f, 1.0f};
    style.Colors[ImGuiCol_ButtonActive] = {0.35f, 0.31f, 0.49f, 1.0f};
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
    ImGui::GetStyle() = style;
    style_scale_ = scale;
  }

  static void draw_scene(
      demo::Scene &scene, ImDrawList &draw, ImVec2 origin, ImVec2 size, float pixels_per_unit
  ) {
    const ImVec2 center{origin.x + size.x / 2.0f, origin.y + size.y / 2.0f};
    constexpr std::array colors{IM_COL32(0xac, 0xff, 0x51, 255), IM_COL32(0xff, 0x51, 0xd9, 255)};

    draw.PushClipRect(origin, {origin.x + size.x, origin.y + size.y}, true);
    for (std::size_t i = 0; i < scene.bodies.size(); ++i) {
      const auto body = scene.world.get_body(scene.bodies[i]);
      const auto shape = body.get_shape();
      if (const auto *circle = std::get_if<fysics::CircleShape>(&shape)) {
        const auto pos = body.get_pos();
        const ImVec2 screen{center.x + pos.x * pixels_per_unit, center.y - pos.y * pixels_per_unit};
        draw.AddCircleFilled(screen, circle->radius * pixels_per_unit, colors[i % colors.size()]);
      }
    }
    draw.PopClipRect();
  }

  static float
  draw_toolbar(demo::Playback &playback, const ImGuiViewport &viewport, double elapsed_seconds) {
    const auto &style = ImGui::GetStyle();
    const float button_width = ImGui::CalcTextSize("Resume").x + 2.0f * style.FramePadding.x;
    const float controls_width = button_width + ImGui::CalcTextSize("Step").x +
                                 ImGui::CalcTextSize("Reset").x + 4.0f * style.FramePadding.x +
                                 2.0f * style.ItemSpacing.x;
    const float time_width = ImGui::CalcTextSize("00000.00000 s").x;
    const float content_width = viewport.Size.x - 2.0f * style.WindowPadding.x;
    const bool time_on_next_row = content_width < controls_width + style.ItemSpacing.x + time_width;
    const float frame_height = ImGui::GetFrameHeight();
    const float height = frame_height + 2.0f * style.WindowPadding.y +
                         (time_on_next_row ? frame_height + style.ItemSpacing.y : 0.0f);
    ImGui::SetNextWindowPos(viewport.Pos);
    ImGui::SetNextWindowSize({viewport.Size.x, height});
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("Playback", nullptr, flags)) {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0x18, 0x22, 0x12, 255));
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0xac, 0xff, 0x51, 255));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0xc5, 0xff, 0x85, 255));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0x96, 0xe8, 0x3c, 255));
      const bool toggle = ImGui::Button(
          playback.paused() ? "Resume###playback" : "Pause###playback", {button_width, 0.0f}
      );
      ImGui::PopStyleColor(4);
      if (toggle)
        playback.toggle_pause();
      ImGui::SameLine();
      ImGui::BeginDisabled(!playback.paused());
      if (ImGui::Button("Step"))
        playback.step_once();
      ImGui::EndDisabled();
      ImGui::SameLine();
      if (ImGui::Button("Reset"))
        playback.reset();
      playback.advance(elapsed_seconds);
      if (!time_on_next_row)
        ImGui::SameLine();
      char time[48];
      std::snprintf(time, sizeof(time), "%.5f s", playback.scene().world.get_time());
      const float right_aligned_x =
          viewport.Size.x - style.WindowPadding.x - ImGui::CalcTextSize(time).x;
      ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), right_aligned_x));
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted(time);
    } else {
      playback.advance(elapsed_seconds);
    }
    ImGui::End();
    return height;
  }

  bool render(demo::Playback &playback, double elapsed_seconds) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    const auto &io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) {
      playback.discard_elapsed();
      SDL_Delay(16);
      return true;
    }

    const float scale = SDL_GetWindowDisplayScale(window_);
    const float density = SDL_GetWindowPixelDensity(window_);
    const float logical_scale = scale > 0.0f && density > 0.0f ? scale / density : 1.0f;
    update_style(logical_scale);
    ImGui::NewFrame();
    const auto *viewport = ImGui::GetMainViewport();
    const float toolbar_height = draw_toolbar(playback, *viewport, elapsed_seconds);
    if (viewport->Size.y > toolbar_height)
      draw_scene(
          playback.scene(),
          *ImGui::GetBackgroundDrawList(),
          {viewport->Pos.x, viewport->Pos.y + toolbar_height},
          {viewport->Size.x, viewport->Size.y - toolbar_height},
          48.0f * logical_scale
      );
    ImGui::Render();

    if (!SDL_SetRenderScale(renderer_, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y))
      return sdl_error("SDL_SetRenderScale");
    if (!SDL_SetRenderDrawColor(renderer_, 0x1e, 0x1b, 0x3a, 255) || !SDL_RenderClear(renderer_))
      return sdl_error("Clearing the viewport");
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
    if (!SDL_RenderPresent(renderer_))
      return sdl_error("SDL_RenderPresent");
    return true;
  }

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  ImGuiContext *imgui_ = nullptr;
  bool platform_backend_ = false;
  bool render_backend_ = false;
  bool vsync_ = false;
  float style_scale_ = 0.0f;
};

} // namespace

int main(int, char **) {
  try {
    Frontend frontend;
    if (!frontend.init())
      return EXIT_FAILURE;
    demo::Playback playback{demo::make_two_circle_scene};
    return frontend.run(playback);
  } catch (const std::exception &error) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fysics demo failed: %s", error.what());
    return EXIT_FAILURE;
  }
}

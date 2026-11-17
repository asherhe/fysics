#include <array>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <variant>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "scene.hpp"

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

  int run(demo::Scene &scene) {
    const SDL_WindowID window_id = SDL_GetWindowID(window_);
    for (;;) {
      const Uint64 frame_start = SDL_GetTicksNS();
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window_id))
          return EXIT_SUCCESS;
      }

      if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) {
        SDL_Delay(16);
        continue;
      }
      if (!render(scene))
        return EXIT_FAILURE;

      constexpr Uint64 frame_duration = SDL_NS_PER_SECOND / 60;
      const Uint64 elapsed = SDL_GetTicksNS() - frame_start;
      if (!vsync_ && elapsed < frame_duration)
        SDL_DelayNS(frame_duration - elapsed);
    }
  }

private:
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

  bool render(demo::Scene &scene) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    const auto &io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) {
      SDL_Delay(16);
      return true;
    }

    const float scale = SDL_GetWindowDisplayScale(window_);
    const float density = SDL_GetWindowPixelDensity(window_);
    const float logical_scale = scale > 0.0f && density > 0.0f ? scale / density : 1.0f;
    ImGui::NewFrame();
    const auto *viewport = ImGui::GetMainViewport();
    draw_scene(
        scene, *ImGui::GetBackgroundDrawList(), viewport->Pos, viewport->Size, 48.0f * logical_scale
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
};

} // namespace

int main(int, char **) {
  try {
    Frontend frontend;
    if (!frontend.init())
      return EXIT_FAILURE;
    auto scene = demo::make_two_circle_scene();
    return frontend.run(scene);
  } catch (const std::exception &error) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Fysics demo failed: %s", error.what());
    return EXIT_FAILURE;
  }
}

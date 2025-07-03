#include "./app.hpp"

#include "SDL3/SDL.h"
#include "gfx/context.hpp"
#include "gfx/pipeline.hpp"
#include "utils/logger.hpp"

#include <print>
#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace wgpu;
namespace fs = std::filesystem;

bool App::Init() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    LOG_ERR("Unable to initialize SDL: {}", SDL_GetError());
    return false;
  }

  size = {800, 600};

  int flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
  win = SDL_CreateWindow("Raymarcher!", size.x, size.y, flags);

  dpiScale = SDL_GetWindowPixelDensity(win);
  fbSize = size * dpiScale;

  LOG_INFO("Window size: {}x{}", size.x, size.y);
  LOG_INFO("DPI scale: {}", dpiScale);
  LOG_INFO("Framebuffer size: {}x{}", fbSize.x, fbSize.y);

  ctx = WGPUContext(win, fbSize, wgpu::PresentMode::Fifo);

  fs::path resourcesDir = ROOT_DIR "/res";
  slang = SlangContext(resourcesDir / "shaders");
  pipeline = Pipeline(slang);

  renderer = Renderer(this);

  return true;
}

void App::MainLoop() {
  if (exit) {
#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#endif
    return;
  }

  // get surface
  renderer.GetNextTexture();

  // events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        LOG_INFO("Quitting...");
        exit = true;
        break;
    }
  }

  // render
  renderer.Begin();
  renderer.RenderTriangle();
  renderer.End();

#ifndef __EMSCRIPTEN__
  ctx.surface.Present();
  ctx.device.Tick();
#endif
}

int main(int argc, char** argv) {
  App app;
  if (!app.Init()) return 1;

#ifndef __EMSCRIPTEN__
  while (!app.exit) {
    app.MainLoop();
  }
#else
  auto callback = [](void* arg) {
    App* app = static_cast<App*>(arg);
    app->MainLoop();
  };
  emscripten_set_main_loop_arg(callback, &app, 0, true);
#endif

  SDL_Quit();

  return 0;
}

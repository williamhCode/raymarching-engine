#include "SDL3/SDL.h"
#include "gfx/camera.hpp"
#include "glm/ext/vector_float2.hpp"

#include "gfx/context.hpp"
#include "gfx/pipeline.hpp"
#include "utils/logger.hpp"
#include "webgpu_utils.hpp"

#include <print>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace wgpu;

struct App {
  bool exit = false;

  SDL_Window* win;
  glm::vec2 size;
  glm::vec2 fbSize;
  float dpiScale;

  Ortho2D camera;
  Buffer vbo;

  bool Init() {
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
    pipeline.Init();

    camera = Ortho2D(size);

    std::vector<BasicQuadVertex> quadData{
      // {{-0.5, -0.5}, {1, 0, 0, 1}},
      // {{0.5, -0.5}, {0, 1, 0, 1}},
      // {{0.5, 0.5}, {0, 0, 1, 1}},
      {{400, 100}, {1, 0, 0, 1}},
      {{650, 500}, {0, 1, 0, 1}},
      {{150, 500}, {0, 0, 1, 1}},
    };

    vbo = ctx.CreateVertexBuffer(
      quadData.size() * sizeof(BasicQuadVertex), quadData.data()
    );

    return true;
  }

  void MainLoop() {
    if (exit) {
#ifdef __EMSCRIPTEN__
      emscripten_cancel_main_loop();
#endif
      return;
    }

    SurfaceTexture surfaceTexture;
    ctx.surface.GetCurrentTexture(&surfaceTexture);
    auto nextTextureView = surfaceTexture.texture.CreateView();

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
    auto commandEncoder = ctx.device.CreateCommandEncoder();
    auto basicRPD = utils::RenderPassDescriptor({
      RenderPassColorAttachment{
        .view = nextTextureView,
        .loadOp = LoadOp::Clear,
        .storeOp = StoreOp::Store,
      },
    });

    RenderPassEncoder passEncoder = commandEncoder.BeginRenderPass(&basicRPD);
    passEncoder.SetPipeline(pipeline.basicRPL);
    passEncoder.SetBindGroup(0, camera.viewProjBG);
    passEncoder.SetVertexBuffer(0, vbo);
    passEncoder.Draw(3);
    passEncoder.End();

    auto commandBuffer = commandEncoder.Finish();
    ctx.queue.Submit(1, &commandBuffer);

#ifndef __EMSCRIPTEN__
    ctx.surface.Present();
#endif
  }
};

int main(int argc, char** argv) {
  App app;

  if (!app.Init()) return 1;

#ifdef __EMSCRIPTEN__
  auto callback = [](void *arg) {
    App *app = reinterpret_cast<App *>(arg);
    app->MainLoop();
  };
  emscripten_set_main_loop_arg(callback, &app, 0, true);
#else
  while (!app.exit) {
    app.MainLoop();
  }
#endif

  SDL_Quit();

  return 0;
}

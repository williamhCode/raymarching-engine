#pragma once

#include "SDL3/SDL_video.h"
#include "gfx/renderer.hpp"
#include "glm/ext/vector_float2.hpp"

struct App {
  bool exit = false;

  SDL_Window* win;
  glm::vec2 size;
  glm::vec2 fbSize;
  float dpiScale;

  Renderer renderer;

  bool Init();
  void MainLoop();
};

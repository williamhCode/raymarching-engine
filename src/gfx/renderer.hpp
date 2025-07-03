#pragma once

#include "gfx/camera.hpp"
#include "webgpu/webgpu_cpp.h"

// forward decl
struct App;

struct Renderer {
  App* app;

  wgpu::BindGroup resolutionBG;
  wgpu::Buffer vbo;

  wgpu::TextureView nextTextureView;
  wgpu::CommandEncoder commandEncoder;

  Renderer() = default;
  Renderer(App* app);

  void GetNextTexture();
  void Begin();
  void RenderTriangle();
  void End();
};

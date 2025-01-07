#pragma once

#include "webgpu/webgpu_cpp.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"

struct BasicQuadVertex {
  glm::vec2 position;
  glm::vec4 color;
};

struct Pipeline {
  wgpu::BindGroupLayout viewProjBGL;
  wgpu::RenderPipeline basicRPL;

  Pipeline() = default;
  void Init();
};

inline Pipeline pipeline;

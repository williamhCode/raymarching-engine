#pragma once

#include "slang_utils/context.hpp"
#include "webgpu/webgpu_cpp.h"
#include "glm/ext/vector_float2.hpp"

struct BasicQuadVertex {
  glm::vec2 position;
};

struct Pipeline {
  wgpu::BindGroupLayout resolutionBGL;
  wgpu::RenderPipeline basicRPL;

  Pipeline() = default;
  Pipeline(SlangContext& slang);
};

inline Pipeline pipeline;

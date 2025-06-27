#include "./pipeline.hpp"

#include "gfx/context.hpp"
#include "webgpu_utils.hpp"

using namespace wgpu;

void Pipeline::Init() {
  viewProjBGL = ctx.MakeBindGroupLayout({
    {0, ShaderStage::Vertex, BufferBindingType::Uniform},
  });

  ShaderModule basicShader = ctx.LoadShaderModulePath(ROOT_DIR "/res/shaders/basic.wgsl");

  basicRPL = ctx.MakeRenderPipeline({
    .vs = basicShader,
    .fs = basicShader,
    .bgls = {viewProjBGL},
    .buffers = {
      {
        .arrayStride = sizeof(BasicQuadVertex),
        .attributes = {
          {VertexFormat::Float32x2, offsetof(BasicQuadVertex, position)},
          {VertexFormat::Float32x4, offsetof(BasicQuadVertex, color)},
        }
      }
    },
    .targets = {{.format = ctx.surfaceFormat}},
  });
}

#include "./pipeline.hpp"

#include "gfx/context.hpp"

using namespace wgpu;

Pipeline::Pipeline(SlangContext& slang) {
  auto loadShaderModule = [&](
    const std::string& moduleName,
    const std::vector<slang::PreprocessorMacroDesc>& macros = {}
  ) {
    std::string source = slang.GetModuleSource(moduleName, macros);
    return ctx.LoadShaderModuleSource(source);
  };

  resolutionBGL = ctx.MakeBindGroupLayout({
    {0, ShaderStage::Fragment, BufferBindingType::Uniform},
  });

  ShaderModule basicShader = loadShaderModule("basic");

  basicRPL = ctx.MakeRenderPipeline({
    .vs = basicShader,
    .fs = basicShader,
    .bgls = {resolutionBGL},
    .buffers = {
      {
        .arrayStride = sizeof(BasicQuadVertex),
        .attributes = {
          {VertexFormat::Float32x2, offsetof(BasicQuadVertex, position)},
        }
      }
    },
    .targets = {{.format = ctx.surfaceFormat}},
  });
}

#include "./renderer.hpp"

#include "app.hpp"
#include "gfx/context.hpp"
#include "gfx/pipeline.hpp"
#include "webgpu_utils/render_pass.hpp"

using namespace wgpu;

Renderer::Renderer(App* _app) : app(_app) {
  Buffer resolutionBuf = ctx.CreateUniformBuffer(
    sizeof(glm::vec2), &app->fbSize
  );
  resolutionBG = ctx.MakeBindGroup(
    pipeline.resolutionBGL,
    {
      {0, resolutionBuf},
    }
  );

  std::vector<BasicQuadVertex> quadData{
    {{-1, 1}},
    {{1, -1}},
    {{1, 1}},

    {{-1, 1}},
    {{-1, -1}},
    {{1, -1}},
  };
  vbo = ctx.CreateVertexBuffer(
    quadData.size() * sizeof(BasicQuadVertex), quadData.data()
  );
}

void Renderer::GetNextTexture() {
  SurfaceTexture surfaceTexture;
  ctx.surface.GetCurrentTexture(&surfaceTexture);
  nextTextureView = surfaceTexture.texture.CreateView();
}

void Renderer::Begin() {
  commandEncoder = ctx.device.CreateCommandEncoder();
}

void Renderer::RenderTriangle() {
  auto basicRPD = utils::RenderPassDescriptor({
    RenderPassColorAttachment{
      .view = nextTextureView,
      .loadOp = LoadOp::Clear,
      .storeOp = StoreOp::Store,
      .clearValue = {0, 0, 0, 1},
    },
  });

  auto passEncoder = commandEncoder.BeginRenderPass(&basicRPD);
  passEncoder.SetPipeline(pipeline.basicRPL);
  passEncoder.SetBindGroup(0, resolutionBG);
  passEncoder.SetVertexBuffer(0, vbo);
  passEncoder.Draw(6);
  passEncoder.End();
}

void Renderer::End() {
  auto commandBuffer = commandEncoder.Finish();
  ctx.queue.Submit(1, &commandBuffer);
  nextTextureView = {};
}

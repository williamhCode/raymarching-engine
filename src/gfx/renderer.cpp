#include "./renderer.hpp"

#include "app.hpp"
#include "gfx/context.hpp"
#include "gfx/pipeline.hpp"
#include "webgpu_utils/render_pass.hpp"

using namespace wgpu;

Renderer::Renderer(App* _app) : app(_app) {
  camera = Ortho2D(app->size);

  std::vector<BasicQuadVertex> quadData{
    {{400, 100}, {1, 0, 0, 1}},
    {{650, 500}, {0, 1, 0, 1}},
    {{150, 500}, {0, 0, 1, 1}},
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
  passEncoder.SetBindGroup(0, camera.viewProjBG);
  passEncoder.SetVertexBuffer(0, vbo);
  passEncoder.Draw(3);
  passEncoder.End();
}

void Renderer::End() {
  auto commandBuffer = commandEncoder.Finish();
  ctx.queue.Submit(1, &commandBuffer);

}

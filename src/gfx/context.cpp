#include "./context.hpp"
#include "utils/logger.hpp"
#include "webgpu_utils/init.hpp"
#include "sdl3webgpu.hpp"
#include "dawn/webgpu_cpp_print.h"
#include <string_view>

using namespace wgpu;

WGPUContext::WGPUContext(SDL_Window* window, glm::uvec2 _size, PresentMode _presentMode)
    : size(_size), presentMode(_presentMode) {

  // instance -----------------------------
#ifndef __EMSCRIPTEN__
  std::vector<std::string> enabledToggles = {
    "enable_immediate_error_handling", "allow_unsafe_apis"
  };

  std::vector<const char*> enabledToggleNames;
  enabledToggleNames.reserve(enabledToggles.size());
  for (const std::string& toggle : enabledToggles) {
    enabledToggleNames.push_back(toggle.c_str());
  }

  DawnTogglesDescriptor toggles({
    .enabledToggleCount = enabledToggleNames.size(),
    .enabledToggles = enabledToggleNames.data(),
  });

  InstanceDescriptor desc{
    .nextInChain = &toggles,
    .capabilities = {.timedWaitAnyEnable = true},
  };
  instance = CreateInstance(&desc);
#else
  InstanceDescriptor desc{
    .capabilities = {.timedWaitAnyEnable = true},
  };
  instance = CreateInstance(&desc);
#endif

  if (!instance) {
    LOG_ERR("Could not create WebGPU instance!");
    std::exit(1);
  }

#ifndef __EMSCRIPTEN__
  surface = SDL_GetWGPUSurface(instance, window);
#else
  EmscriptenSurfaceSourceCanvasHTMLSelector canvasDesc({
    .selector = "#canvas",
  });

  SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  surface = instance.CreateSurface(&surfaceDesc);
#endif

  // adapter ------------------------------------
  adapter = utils::RequestAdapter(
    instance,
    {
      .compatibleSurface = surface,
    }
  );

  // SupportedLimits supportedLimits;
  // adapter.GetLimits(&supportedLimits);
  // utils::PrintLimits(supportedLimits.limits);

  // FeatureName features[256];
  // size_t featureCount = adapter.EnumerateFeatures(features);
  // for (size_t i = 0; i < featureCount; i++) {
  //   LOG_INFO("feature: {}", magic_enum::enum_name(features[i]));
  // }

  // RequiredLimits requiredLimits{
  //   .limits = supportedLimits.limits,
  // };

  // SurfaceCapabilities surfaceCaps;
  // surface.GetCapabilities(adapter, &surfaceCaps);
  // utils::PrintSurfaceCapabilities(surfaceCaps);

  // device -------------------------------------
  // std::vector<FeatureName> requiredFeatures{
  //   FeatureName::TimestampQuery,
  // };
  // DeviceDescriptor deviceDesc({
  //   .requiredFeatureCount = requiredFeatures.size(),
  //   .requiredFeatures = requiredFeatures.data(),
  // });

  DeviceDescriptor deviceDesc;

  deviceDesc.SetUncapturedErrorCallback(
    [](const Device& _, ErrorType type, StringView message) {
      std::ostringstream() << type;
      LOG_ERR("Device error: {} ({})", ToString(type), std::string_view(message));
      // LOG_TRACE();
    }
  );

  deviceDesc.SetDeviceLostCallback(
    CallbackMode::AllowSpontaneous,
    [](const Device& _, DeviceLostReason reason, StringView message) {
      if (reason != DeviceLostReason::Destroyed) {
        LOG_ERR("Device lost: {} ({})", ToString(reason), std::string_view(message));
      }
    }
  );

  device = utils::RequestDevice(instance, adapter, deviceDesc);
  DeviceWrapper::device = device;

  queue = device.GetQueue();

  // surface --------------------------------
  surfaceFormat = TextureFormat::BGRA8Unorm;

  // apple doesn't support unpremultiplied alpha
  alphaMode = CompositeAlphaMode::Premultiplied;

  SurfaceConfiguration surfaceConfig{
    .device = device,
    .format = surfaceFormat,
    .width = size.x,
    .height = size.y,
    .alphaMode = alphaMode,
    .presentMode = presentMode,
  };
  surface.Configure(&surfaceConfig);
}

void WGPUContext::Resize(glm::uvec2 _size) {
  size = _size;

  SurfaceConfiguration surfaceConfig{
    .device = device,
    .format = surfaceFormat,
    .width = size.x,
    .height = size.y,
    .alphaMode = alphaMode,
    .presentMode = presentMode,
  };
  surface.Configure(&surfaceConfig);
}


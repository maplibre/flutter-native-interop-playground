#pragma once

#include <cstdint>

#include "renderer_backend.hpp"

namespace flmbgl {
class FlutterTextureInterface {
 public:
  FlutterTextureInterface() = default;
  virtual ~FlutterTextureInterface() = default;

  static std::unique_ptr<FlutterTextureInterface> Create();

  // Returns the associated FlutterTexture id.
  virtual int64_t getTextureId() const = 0;
  virtual void update(flmbgl::RendererBackend& backend) = 0;
};
}  // namespace flmbgl
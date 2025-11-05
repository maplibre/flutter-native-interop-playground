#pragma once

#include "../../flutter_texture_interface.hpp"
#include "../../renderer_backend.hpp"

namespace flmln {
namespace android {
class FlutterTextureInterface : public flmln::FlutterTextureInterface {
 public:
  FlutterTextureInterface() {}
  ~FlutterTextureInterface() override {}

  int64_t getTextureId() const override { return -1; }
  void update(flmln::RendererBackend& backend) override {}
};
}  // namespace android
}  // namespace flmln
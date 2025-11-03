#include "flutter_texture_interface.hpp"

#ifdef FLMBGL_MACOS
#include "platform/darwin/flutter_texture_interface.hpp"
#endif

namespace flmbgl {
std::unique_ptr<FlutterTextureInterface> FlutterTextureInterface::Create() {
#ifdef FLMBGL_MACOS
  return std::make_unique<macos::FlutterTextureInterface>();
#endif
  return nullptr;
}
}  // namespace flmbgl
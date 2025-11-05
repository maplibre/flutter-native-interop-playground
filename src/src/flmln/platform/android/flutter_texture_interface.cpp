#include "flutter_texture_interface.hpp"

#include "../../flutter_texture_interface.hpp"

namespace flmln {
std::unique_ptr<flmln::FlutterTextureInterface> FlutterTextureInterface::Create() {
  return std::make_unique<flmln::android::FlutterTextureInterface>();
}
}  // namespace flmln
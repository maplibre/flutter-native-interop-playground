#include "renderer_backend.hpp"

#ifdef FLMBGL_MTL
#include "mtl/renderer_backend.hpp"
#endif

namespace flmbgl {
std::unique_ptr<RendererBackend> RendererBackend::create(mbgl::gfx::ContextMode contextMode) {
#ifdef FLMBGL_MTL
  return std::make_unique<mtl::RendererBackend>(contextMode);
#endif
  return nullptr;
}
}  // namespace flmbgl

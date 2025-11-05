#include "renderer_backend.hpp"

#include "../renderer_backend.hpp"

namespace flmln {
std::unique_ptr<flmln::RendererBackend> RendererBackend::create(mbgl::gfx::ContextMode contextMode) {
  return std::make_unique<flmln::mtl::RendererBackend>(contextMode);
}
}  // namespace flmln
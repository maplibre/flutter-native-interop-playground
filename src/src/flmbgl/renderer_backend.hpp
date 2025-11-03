#pragma once

#include <mbgl/gfx/offscreen_texture.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/size.hpp>

namespace flmbgl {
class RendererBackend : public mbgl::gfx::Renderable {
 public:
  RendererBackend() : mbgl::gfx::Renderable({0, 0}, nullptr) {}
  virtual ~RendererBackend() = default;

  static std::unique_ptr<RendererBackend> create(mbgl::gfx::ContextMode contextMode);

  virtual mbgl::gfx::RendererBackend* getRendererBackend() = 0;
  virtual mbgl::gfx::Texture2DPtr getTexture() const = 0;
  virtual mbgl::PremultipliedImage readStillImage() = 0;

  void setSize(mbgl::Size size_) {
    size = size_;
    resource.reset();
  }
};
}  // namespace flmbgl
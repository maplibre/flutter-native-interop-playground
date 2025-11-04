#pragma once

#include <EGL/egl.h>

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gl/texture2d.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/util.hpp>

#include "../renderer_backend.hpp"

namespace flmln {
namespace gl {
class RenderableResource : public mbgl::gl::RenderableResource {
 public:
  RenderableResource(mbgl::gl::Context& context_, mbgl::Size size) : context(context_) {
    offscreenTexture = context.createOffscreenTexture(size, mbgl::gfx::TextureChannelDataType::UnsignedByte);
  }

  ~RenderableResource() noexcept override = default;

  // -- Overrides --
  void bind() override { offscreenTexture->getResource<mbgl::gl::RenderableResource>().bind(); }
  void swap() override { offscreenTexture->getResource<mbgl::gl::RenderableResource>().swap(); }

  // -- New methods --
  mbgl::gfx::Texture2DPtr getTexture() const { return offscreenTexture->getTexture(); }
  mbgl::PremultipliedImage readStillImage() { return offscreenTexture->readStillImage(); }

 private:
  mbgl::gl::Context& context;
  std::unique_ptr<mbgl::gfx::OffscreenTexture> offscreenTexture;
};

class RendererBackend : public flmln::RendererBackend, public mbgl::gl::RendererBackend {
 public:
  RendererBackend(mbgl::gfx::ContextMode contextMode)
      : flmln::RendererBackend(), mbgl::gl::RendererBackend(contextMode) {}

  ~RendererBackend() noexcept override {
    mbgl::gfx::BackendScope guard{*this, mbgl::gfx::BackendScope::ScopeType::Implicit};
    context.reset();
    resource.reset();
  }

  mbgl::gl::ProcAddress getExtensionFunctionPointer(const char* name) override { return eglGetProcAddress(name); }

  mbgl::gfx::RendererBackend* getRendererBackend() override { return this; }
  mbgl::gfx::Renderable& getDefaultRenderable() override {
    if (!resource) {
      resource = std::make_unique<RenderableResource>(static_cast<mbgl::gl::Context&>(getContext()), size);
    }

    return *this;
  }

  mbgl::gfx::Texture2DPtr getTexture() const override {
    if (resource) {
      auto renderableResource = static_cast<RenderableResource*>(resource.get());
      if (renderableResource) return renderableResource->getTexture();
    }

    return nullptr;
  }

  mbgl::platform::GLuint getGLTextureID() const {
    auto texturePtr = getTexture();
    if (!texturePtr) return 0;

    auto glTexture = static_cast<mbgl::gl::Texture2D*>(texturePtr.get());
    if (!glTexture) return 0;

    return glTexture->getTextureID();
  }

  mbgl::PremultipliedImage readStillImage() override {
    if (resource) {
      auto renderableResource = static_cast<RenderableResource*>(resource.get());
      if (renderableResource) return renderableResource->readStillImage();
    }

    return mbgl::PremultipliedImage{};
  }

  void activate() override { active = true; }
  void deactivate() override { active = false; }
  void updateAssumedState() override {}

 private:
  bool active = false;
};
}  // namespace gl
}  // namespace flmln

namespace mbgl {
namespace gl {
void HeadlessBackend::createImpl() {
  assert(!impl);
}
}  // namespace gl
}  // namespace mbgl
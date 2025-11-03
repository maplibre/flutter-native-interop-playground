#pragma once

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/mtl/headless_backend.hpp>
#include <mbgl/mtl/renderable_resource.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>

#include "flutter_texture_interface.hpp"
#include "renderer_backend.hpp"

namespace flmbgl {
class RendererFrontend : public mbgl::RendererFrontend {
 public:
  RendererFrontend(const std::optional<std::string>& localFontFamily) : size({0, 0}), pixelRatio(0.0f) {
    backend = flmbgl::RendererBackend::create(mbgl::gfx::ContextMode::Unique);
    renderer = std::make_unique<mbgl::Renderer>(*getBackend(), 2.0f, localFontFamily);
    textureInterface = flmbgl::FlutterTextureInterface::Create();
  }

  // Return the RendererBackend for this frontend.
  mbgl::gfx::RendererBackend* getBackend() { return backend->getRendererBackend(); }

  // Set the size and pixel ratio for the backend. If the same values are passed, does nothing.
  void setSizeAndPixelRatio(mbgl::Size size, float pixelRatio) {
    if (this->size != size || this->pixelRatio != pixelRatio) {
      this->size = size;
      this->pixelRatio = pixelRatio;
      auto scaledSize = mbgl::Size{
          static_cast<uint32_t>(size.width * pixelRatio),
          static_cast<uint32_t>(size.height * pixelRatio),
      };

      backend->setSize(scaledSize);
    }
  }

  // Render a frame using the current update parameters.
  void renderFrame() {
    if (renderer && updateParameters) {
      mbgl::gfx::BackendScope guard{*getBackend()};

      auto updateParameters_ = updateParameters;
      renderer->render(updateParameters_);

      auto texture = backend->getTexture();
      textureInterface->update(*backend);
    }
  }

  void reset() override {
    assert(renderer);
    renderer.reset();
  }

  void update(std::shared_ptr<mbgl::UpdateParameters> updateParameters_) override {
    updateParameters = updateParameters_;
  }

  const mbgl::TaggedScheduler& getThreadPool() const override { return backend->getRendererBackend()->getThreadPool(); }

  void setObserver(mbgl::RendererObserver& observer_) override {
    assert(renderer);
    renderer->setObserver(&observer_);
  }

  int64_t getTextureId() const { return textureInterface->getTextureId(); }

 private:
  mbgl::Size size;
  float pixelRatio;
  std::unique_ptr<flmbgl::RendererBackend> backend;
  std::unique_ptr<flmbgl::FlutterTextureInterface> textureInterface;
  std::unique_ptr<mbgl::Renderer> renderer;
  std::shared_ptr<mbgl::UpdateParameters> updateParameters;
};
}  // namespace flmbgl

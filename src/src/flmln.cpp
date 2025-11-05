#include "flmln.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/run_loop.hpp>

#include "flmln/map_observer.hpp"
#include "flmln/renderer_frontend.hpp"
#include "flmln/platform.hpp"

int test_flmln() { return 42; }

void flmln_initialize() { flmln::initializePlatform(); }

// ---------------------------------
// mbgl_map_options_t
// ---------------------------------

mbgl_map_options_t mbgl_map_options_create() { return reinterpret_cast<mbgl_map_options_t>(new mbgl::MapOptions()); }

void mbgl_map_options_destroy(mbgl_map_options_t _mapOptions) {
  delete reinterpret_cast<mbgl::MapOptions*>(_mapOptions);
}

void mbgl_map_options_set_mode(mbgl_map_options_t _mapOptions, enum MbglMapMode mode) {
  auto* mapOptions = reinterpret_cast<mbgl::MapOptions*>(_mapOptions);
  mapOptions->withMapMode(static_cast<mbgl::MapMode>(mode));
}

void mbgl_map_options_set_size(mbgl_map_options_t _mapOptions, uint32_t width, uint32_t height) {
  auto* mapOptions = reinterpret_cast<mbgl::MapOptions*>(_mapOptions);
  mapOptions->withSize({width, height});
}

void mbgl_map_options_set_pixel_ratio(mbgl_map_options_t _mapOptions, float pixelRatio) {
  auto* mapOptions = reinterpret_cast<mbgl::MapOptions*>(_mapOptions);
  mapOptions->withPixelRatio(pixelRatio);
}

// ----------------------------------
// mbgl_tile_server_options_t
// ----------------------------------

mbgl_tile_server_options_t mbgl_tile_server_options_create() {
  return reinterpret_cast<mbgl_tile_server_options_t>(new mbgl::TileServerOptions());
}

mbgl_tile_server_options_t mbgl_tile_server_options_mapbox_configuration_create() {
  auto* configuration = new auto(mbgl::TileServerOptions::MapboxConfiguration());
  return reinterpret_cast<mbgl_tile_server_options_t>(configuration);
}

mbgl_tile_server_options_t mbgl_tile_server_options_maptiler_configuration_create() {
  auto* configuration = new auto(mbgl::TileServerOptions::MapTilerConfiguration());
  return reinterpret_cast<mbgl_tile_server_options_t>(configuration);
}

mbgl_tile_server_options_t mbgl_tile_server_options_maplibre_configuration_create() {
  auto* configuration = new auto(mbgl::TileServerOptions::MapLibreConfiguration());
  return reinterpret_cast<mbgl_tile_server_options_t>(configuration);
}

void mbgl_tile_server_options_destroy(mbgl_tile_server_options_t _tileServerOptions) {
  delete reinterpret_cast<mbgl::TileServerOptions*>(_tileServerOptions);
}

// ---------------------------------
// mbgl_resource_options_t
// ---------------------------------

mbgl_resource_options_t mbgl_resource_options_create() {
  return reinterpret_cast<mbgl_resource_options_t>(new mbgl::ResourceOptions());
}

void mbgl_resource_options_destroy(mbgl_resource_options_t _resourceOptions) {
  delete reinterpret_cast<mbgl::ResourceOptions*>(_resourceOptions);
}

void mbgl_resource_options_set_api_key(mbgl_resource_options_t _resourceOptions, const char* apiKey) {
  auto* resourceOptions = reinterpret_cast<mbgl::ResourceOptions*>(_resourceOptions);
  resourceOptions->withApiKey(std::string(apiKey));
}

void mbgl_resource_options_set_tile_server_options(mbgl_resource_options_t _resourceOptions,
                                                   mbgl_tile_server_options_t _tileServerOptions) {
  auto* resourceOptions = reinterpret_cast<mbgl::ResourceOptions*>(_resourceOptions);
  auto* tileServerOpts = reinterpret_cast<mbgl::TileServerOptions*>(_tileServerOptions);
  resourceOptions->withTileServerOptions(*tileServerOpts);
}

// ---------------------------------
// flmln_map_observer_t
// ---------------------------------

flmln_map_observer_t flmln_map_observer_create() {
  return reinterpret_cast<flmln_map_observer_t>(new flmln::MapObserver());
}

void flmln_map_observer_destroy(flmln_map_observer_t _mapObserver) {
  delete reinterpret_cast<flmln::MapObserver*>(_mapObserver);
}

// ---------------------------------
// flmln_renderer_frontend_t
// ---------------------------------

flmln_renderer_frontend_t flmln_renderer_frontend_create() {
  return reinterpret_cast<flmln_renderer_frontend_t>(new flmln::RendererFrontend(std::nullopt));
}

void flmln_renderer_frontend_destroy(flmln_renderer_frontend_t _rendererFrontend) {
  delete reinterpret_cast<flmln::RendererFrontend*>(_rendererFrontend);
}

void flmln_renderer_frontend_render(flmln_renderer_frontend_t _rendererFrontend) {
  auto* rendererFrontend = reinterpret_cast<flmln::RendererFrontend*>(_rendererFrontend);
  mbgl::util::RunLoop::Get()->runOnce();
  rendererFrontend->renderFrame();
}

void flmln_renderer_frontend_set_size_and_pixel_ratio(flmln_renderer_frontend_t _rendererFrontend, uint32_t width,
                                                      uint32_t height, float pixelRatio) {
  auto* rendererFrontend = reinterpret_cast<flmln::RendererFrontend*>(_rendererFrontend);
  rendererFrontend->setSizeAndPixelRatio({static_cast<uint32_t>(width), static_cast<uint32_t>(height)}, pixelRatio);
}

int64_t flmln_renderer_frontend_get_texture_id(flmln_renderer_frontend_t _rendererFrontend) {
  auto* rendererFrontend = reinterpret_cast<flmln::RendererFrontend*>(_rendererFrontend);
  return rendererFrontend->getTextureId();
}

// ---------------------------------
// mbgl_camera_options_t
// ---------------------------------

mbgl_camera_options_t mbgl_camera_options_create() {
  return reinterpret_cast<mbgl_camera_options_t>(new mbgl::CameraOptions());
}

void mbgl_camera_options_destroy(mbgl_camera_options_t _cameraOptions) {
  delete reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
}

void mbgl_camera_options_set_center(mbgl_camera_options_t _cameraOptions, double latitude, double longitude) {
  auto* cameraOptions = reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
  cameraOptions->withCenter(mbgl::LatLng(latitude, longitude));
}

void mbgl_camera_options_set_zoom(mbgl_camera_options_t _cameraOptions, double zoom) {
  auto* cameraOptions = reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
  cameraOptions->withZoom(zoom);
}

void mbgl_camera_options_set_bearing(mbgl_camera_options_t _cameraOptions, double bearing) {
  auto* cameraOptions = reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
  cameraOptions->withBearing(bearing);
}

void mbgl_camera_options_set_pitch(mbgl_camera_options_t _cameraOptions, double pitch) {
  auto* cameraOptions = reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
  cameraOptions->withPitch(pitch);
}

// ---------------------------------
// mbgl_map_t
// ---------------------------------

mbgl_map_t mbgl_map_create(flmln_renderer_frontend_t _rendererFrontend, flmln_map_observer_t _mapObserver,
                           mbgl_map_options_t _mapOptions, mbgl_resource_options_t _resourceOptions) {
  auto* rendererFrontend = reinterpret_cast<flmln::RendererFrontend*>(_rendererFrontend);
  auto* mapObserver = reinterpret_cast<flmln::MapObserver*>(_mapObserver);
  auto* mapOptions = reinterpret_cast<mbgl::MapOptions*>(_mapOptions);
  auto* resourceOptions = reinterpret_cast<mbgl::ResourceOptions*>(_resourceOptions);

  auto map = new mbgl::Map(*rendererFrontend, *mapObserver, *mapOptions, *resourceOptions);
  return reinterpret_cast<mbgl_map_t>(map);
}

void mbgl_map_destroy(mbgl_map_t _map) { delete reinterpret_cast<mbgl::Map*>(_map); }

void mbgl_map_style_load_url(mbgl_map_t _map, const char* url) {
  auto* map = reinterpret_cast<mbgl::Map*>(_map);
  map->getStyle().loadURL(std::string(url));
}

void mbgl_map_jump_to(mbgl_map_t _map, mbgl_camera_options_t _cameraOptions) {
  auto* map = reinterpret_cast<mbgl::Map*>(_map);
  auto* cameraOptions = reinterpret_cast<mbgl::CameraOptions*>(_cameraOptions);
  map->jumpTo(*cameraOptions);
}

void mbgl_map_set_size(mbgl_map_t _map, uint32_t width, uint32_t height) {
  auto* map = reinterpret_cast<mbgl::Map*>(_map);
  map->setSize({width, height});
}

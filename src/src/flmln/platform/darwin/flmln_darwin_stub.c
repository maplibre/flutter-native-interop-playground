#include <stdint.h>
#include <stddef.h>

typedef void* texture_registry_t;
typedef void* flmln_flutter_texture_t;
typedef void* flmln_metal_flutter_texture_t;

texture_registry_t FlMlnDarwinPluginGlobals_getTextureRegistry() { return NULL; }

// FlutterTextureRegistryProxy.swift
int64_t FlutterTextureRegistryProxy_registerTexture(texture_registry_t registry, void* texture) { return -1; }
void FlutterTextureRegistryProxy_textureFrameAvailable(texture_registry_t registry, int64_t textureId) { return; }
void FlutterTextureRegistryProxy_unregisterTexture(texture_registry_t registry, int64_t textureId) { return; }

// FlMlnMetalFlutterTexture.swift
flmln_metal_flutter_texture_t FlMlnMetalFlutterTexture_create() { return NULL; }
void FlMlnMetalFlutterTexture_destroy(flmln_metal_flutter_texture_t texture) { return; }
void FlMlnMetalFlutterTexture_updateBuffer(flmln_metal_flutter_texture_t texture, void* mtlTexture) { return; }

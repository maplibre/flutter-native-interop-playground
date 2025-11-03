import Foundation

#if canImport(Flutter)
  import Flutter
#endif

#if canImport(FlutterMacOS)
  import FlutterMacOS
#endif

@_cdecl("FlMbglDarwinPluginGlobals_getTextureRegistry")
public func FlMbglDarwinPluginGlobals_getTextureRegistry() -> UnsafeRawPointer? {
  guard let proxy = FlMbglDarwinPluginGlobals.textureRegistry else {
    return nil
  }
  return UnsafeRawPointer(Unmanaged.passUnretained(proxy).toOpaque())
}

@objc public class FlMbglDarwinPluginGlobals: NSObject {
  @objc public static var textureRegistry: FlutterTextureRegistryProxy?
}

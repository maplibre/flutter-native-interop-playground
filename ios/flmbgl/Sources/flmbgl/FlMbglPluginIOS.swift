import Flutter
import Foundation

public class FlMbglPluginIOS: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let instance = FlMbglPluginIOS()
    FlMbglDarwinPluginGlobals.textureRegistry = FlutterTextureRegistryProxy(textures: registrar.textures())
  }
}

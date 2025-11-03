import CoreImage
import Foundation

#if canImport(Flutter)
  import Flutter
#endif

#if canImport(FlutterMacOS)
  import FlutterMacOS
#endif

@_cdecl("FlMbglFlutterTexture_create")
public func FlMbglFlutterTexture_create() -> UnsafeRawPointer {
  let texture = FlMbglFlutterTexture()
  return UnsafeRawPointer(Unmanaged.passRetained(texture).toOpaque())
}

@_cdecl("FlMbglFlutterTexture_destroy")
public func FlMbglFlutterTexture_destroy(texturePtr: UnsafeRawPointer) {
  Unmanaged<FlMbglFlutterTexture>.fromOpaque(texturePtr).release()
}

@_cdecl("FlMbglFlutterTexture_createBuffer")
public func FlMbglFlutterTexture_createBuffer(
  texturePtr: UnsafeRawPointer,
  width: UInt32,
  height: UInt32,
  channels: UInt32
) {
  let texture = Unmanaged<FlMbglFlutterTexture>.fromOpaque(texturePtr).takeUnretainedValue()
  texture.createBuffer(width: Int(width), height: Int(height), channels: Int(channels))
}

@_cdecl("FlMbglFlutterTexture_setBufferData")
public func FlMbglFlutterTexture_setBufferData(
  texturePtr: UnsafeRawPointer,
  dataPtr: UnsafeRawPointer,
  dataLength: size_t
) {
  let texture = Unmanaged<FlMbglFlutterTexture>.fromOpaque(texturePtr).takeUnretainedValue()
  texture.setBufferData(dataPtr: dataPtr, dataLength: dataLength)
}

@objc public class FlMbglFlutterTexture: NSObject, FlutterTexture {
  public override init() {
    super.init()
  }

  private var width: Int? = nil
  private var height: Int? = nil
  private var channels: Int? = nil

  private var pool: CVPixelBufferPool?
  private var buffer: CVPixelBuffer?

  @objc public func createBuffer(width: Int, height: Int, channels: Int) {
    if self.width == width && self.height == height && self.channels == channels { return }
    self.width = width
    self.height = height
    self.channels = channels

    let dictionary =
      [
        kCVPixelBufferPixelFormatTypeKey: kCVPixelFormatType_32BGRA,
        kCVPixelBufferWidthKey: width,
        kCVPixelBufferHeightKey: height,
        kCVPixelBufferBytesPerRowAlignmentKey: 4,
        kCVPixelBufferCGBitmapContextCompatibilityKey: true,
        kCVPixelBufferCGImageCompatibilityKey: true,
        kCVPixelBufferMetalCompatibilityKey: true,
        kCVPixelBufferOpenGLCompatibilityKey: true,
      ] as [CFString: Any]

    CVPixelBufferPoolCreate(kCFAllocatorDefault, nil, dictionary as CFDictionary, &self.pool)
    CVPixelBufferPoolCreatePixelBuffer(kCFAllocatorDefault, self.pool!, &self.buffer)

    // print("Created CVPixelBuffer of size \(width)x\(height) with \(channels) channels")
  }

  @objc public func setBufferData(dataPtr: UnsafeRawPointer, dataLength: size_t) {
    guard let buffer = self.buffer else { return }

    CVPixelBufferLockBaseAddress(buffer, [])
    guard let baseAddress = CVPixelBufferGetBaseAddress(buffer) else {
      // print("Failed to get base address of CVPixelBuffer")
      CVPixelBufferUnlockBaseAddress(buffer, [])
      return
    }

    let bytesPerRow = CVPixelBufferGetBytesPerRow(buffer)
    let srcBytesPerRow = self.width! * self.channels!

    guard dataLength == srcBytesPerRow * self.height! else {
      // print(
      //   "Data length \(dataLength) does not match expected size \(srcBytesPerRow * self.height!)")
      CVPixelBufferUnlockBaseAddress(buffer, [])
      return
    }

    let srcAddress = dataPtr.bindMemory(to: UInt8.self, capacity: dataLength)
    let destAddress = baseAddress.bindMemory(to: UInt8.self, capacity: bytesPerRow * self.height!)

    if srcBytesPerRow == bytesPerRow {
      memcpy(baseAddress, dataPtr, dataLength)
    } else {
      for y in 0..<self.height! {
        let srcRow = srcAddress.advanced(by: y * srcBytesPerRow)
        let destRow = destAddress.advanced(by: y * bytesPerRow)
        memcpy(destRow, srcRow, srcBytesPerRow)
      }
    }

    // print("Copied \(dataLength) bytes to CVPixelBuffer")
    CVPixelBufferUnlockBaseAddress(buffer, [])
  }

  public func copyPixelBuffer() -> Unmanaged<CVPixelBuffer>? {
    guard let buffer = self.buffer else { return nil }
    return Unmanaged.passRetained(buffer)
  }
}

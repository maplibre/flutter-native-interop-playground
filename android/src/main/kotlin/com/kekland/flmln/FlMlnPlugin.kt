package com.kekland.flmln

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

import android.util.Log

class FlMlnPlugin : FlutterPlugin, MethodCallHandler {
    private lateinit var channel : MethodChannel

    override fun onAttachedToEngine(flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
        Log.i("FlMlnPlugin", "Loading native library")
        System.loadLibrary("c++_shared")
        Log.i("FlMlnPlugin", "C++ std shared library loaded")
        System.loadLibrary("flmln")
        Log.i("FlMlnPlugin", "Native library loaded")

        channel = MethodChannel(flutterPluginBinding.binaryMessenger, "flmln")
        channel.setMethodCallHandler(this)
    }

    override fun onMethodCall(call: MethodCall, result: Result) {
      if (call.method == "getPlatformVersion") {
        result.success("Android ${android.os.Build.VERSION.RELEASE}")
      } else {
        result.notImplemented()
      }
    }

    override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        // no-op
    }
}

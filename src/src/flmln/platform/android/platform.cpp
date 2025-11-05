#include "../../platform.hpp"

#include <bitmap.hpp>
#include <bitmap_factory.hpp>
#include <java/util.hpp>
#include <java_types.hpp>
#include <jni_native.hpp>
#include <logger.hpp>
#include <text/local_glyph_rasterizer_jni.hpp>

#include "jni.hpp"

JavaVM* flmln_jvm = nullptr;

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
  assert(vm != nullptr);
  mbgl::android::theJVM = vm;
  flmln_jvm = vm;
  return JNI_VERSION_1_6;
}

static void SetContextClassLoader(JNIEnv* env, jobject appContext) {
  // appContext.getClassLoader()
  jclass ctxCls = env->GetObjectClass(appContext);
  jmethodID midGetCL = env->GetMethodID(ctxCls, "getClassLoader", "()Ljava/lang/ClassLoader;");
  jobject appCl = env->CallObjectMethod(appContext, midGetCL);

  // Thread.currentThread().setContextClassLoader(appCl)
  jclass threadCls = env->FindClass("java/lang/Thread");
  jmethodID midCur = env->GetStaticMethodID(threadCls, "currentThread", "()Ljava/lang/Thread;");
  jobject cur = env->CallStaticObjectMethod(threadCls, midCur);

  jclass curCls = env->GetObjectClass(cur);
  jmethodID midSet = env->GetMethodID(curCls, "setContextClassLoader", "(Ljava/lang/ClassLoader;)V");
  env->CallVoidMethod(cur, midSet, appCl);
}

void initializeJNI(JNIEnv* _env) {
  jni::JNIEnv& env = *_env;
  mbgl::android::java::registerNatives(env);
  mbgl::android::java::util::registerNative(env);
  mbgl::android::LocalGlyphRasterizer::registerNative(env);
//   mbgl::android::RegisterNativeHTTPRequest(env);

  mbgl::android::Bitmap::registerNative(env);
  mbgl::android::BitmapFactory::registerNative(env);

  mbgl::android::Logger::registerNative(env);
}

extern "C" JNIEXPORT void JNICALL Java_com_kekland_flmln_NativeBridge_nativeInitialize(
    JNIEnv* env, jclass cls, jobject appContext) {
  SetContextClassLoader(env, appContext);
  initializeJNI(env);
}

namespace flmln {
void initializePlatform() {
}
}  // namespace flmln

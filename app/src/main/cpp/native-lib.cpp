#include <jni.h>
#include <string>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <android/log.h>

#include "renderer.h"

extern "C"
JNIEXPORT void JNICALL
Java_gltest_tdkn_com_gltestapplication_MainRenderer_nativeOnSurfaceCreated(JNIEnv *env, jclass type) {
    on_init();
}

extern "C"
JNIEXPORT void JNICALL
Java_gltest_tdkn_com_gltestapplication_MainRenderer_nativeOnSurfaceChanged(JNIEnv *env, jclass type,
                                                                           jint width,
                                                                           jint height) {
    on_surface_changed(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_gltest_tdkn_com_gltestapplication_MainRenderer_nativeOnDrawFrame(JNIEnv *env, jclass type) {
    on_update();
    on_draw();
}

extern "C"
JNIEXPORT void JNICALL
Java_gltest_tdkn_com_gltestapplication_MainRenderer_nativeMoveCamera(JNIEnv *env, jclass type, jfloat diffX, jfloat diffY) {
    move_camera(diffX, diffY);
}

extern "C"
JNIEXPORT void JNICALL
Java_gltest_tdkn_com_gltestapplication_MainRenderer_nativeRotateCamera(JNIEnv *env, jclass type, jfloat rotation) {
    rotate_camera(rotation);
}

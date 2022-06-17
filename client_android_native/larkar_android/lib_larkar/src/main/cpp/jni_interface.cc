//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <sys/mman.h>

#include "ar_application.h"

extern "C" {
// maintain a reference to the JVM so we can use it later.
static JavaVM *g_vm = nullptr;
static jobject g_ar_activity_ = nullptr;

inline jlong jptr(ArApplication *native_ar_demo_application) {
    return reinterpret_cast<intptr_t>(native_ar_demo_application);
}

inline ArApplication *Native(jlong ptr) {
    return reinterpret_cast<ArApplication *>(ptr);
}

class EnvWrapper {
public:
    EnvWrapper(JavaVM * vm, JNIEnv * env, bool needAttach)
        :vm_(vm), env_(env), need_attach_(needAttach) {
        if (need_attach_) {
            vm_->AttachCurrentThread(&env_, nullptr);
        }
    }

    ~EnvWrapper() {
        if (need_attach_ && env_ != nullptr) {
            vm_->DetachCurrentThread();
        }
    };

    inline JNIEnv * get() {
        return env_;
    }
private:
    JavaVM* vm_;
    JNIEnv* env_;
    bool need_attach_;
};

jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNIEnv *GetJniEnv() {
    JNIEnv *env = nullptr;
    bool detached = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6) == JNI_EDETACHED;
    if (detached) g_vm->AttachCurrentThread(&env, nullptr);
    return env;
}

jclass FindClass(const char *classname) {
    JNIEnv *env = GetJniEnv();
    return env->FindClass(classname);
}

EnvWrapper GetEnvWrapper() {
    JNIEnv *env = nullptr;
    bool detached = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6) == JNI_EDETACHED;
    return EnvWrapper(g_vm, env, detached);
}

void ArActivity_Toast(const char* message) {
    if (g_ar_activity_ == nullptr) {
        return;
    }
    EnvWrapper env = GetEnvWrapper();
    jclass clazz = env.get()->GetObjectClass(g_ar_activity_);
    jmethodID mid = env.get()->GetMethodID(clazz, "showToast", "(Ljava/lang/String;)V");

    jstring jstr = env.get()->NewStringUTF(message);
    env.get()->CallVoidMethod(g_ar_activity_, mid, jstr);

    env.get()->DeleteLocalRef(jstr);
    env.get()->DeleteLocalRef(clazz);
}

void ArActivity_Error(const char* error) {
    if (g_ar_activity_ == nullptr) {
        return;
    }
    EnvWrapper env = GetEnvWrapper();
    jclass clazz = env.get()->GetObjectClass(g_ar_activity_);
    jmethodID mid = env.get()->GetMethodID(clazz, "onError", "(Ljava/lang/String;)V");

    jstring jstr = env.get()->NewStringUTF(error);
    env.get()->CallVoidMethod(g_ar_activity_, mid, jstr);

    env.get()->DeleteLocalRef(jstr);
    env.get()->DeleteLocalRef(clazz);
}
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_pxy_liblarkar_JniInterface_createNativeApplication(JNIEnv *env, jclass clazz,
                                                              jobject arActivity,jstring appid,
                                                            jint sdk_type) {
//    mprotect(2);

    const char *c_appid = env->GetStringUTFChars(appid, 0);

    // save global context
    g_ar_activity_ = env->NewGlobalRef(arActivity);

    auto *app = new ArApplication(c_appid, sdk_type);

    env->ReleaseStringUTFChars(appid, c_appid);
    return jptr(app);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onResume(JNIEnv *env, jclass clazz,
                                                   jlong native_application, jobject context,
                                                   jobject activity) {
    Native(native_application)->OnResume(env, context, activity);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onDisplayGeometryChanged(JNIEnv *env, jclass clazz,
                                                                   jlong native_application,
                                                                   jint display_rotation,
                                                                   jint width, jint height) {
    Native(native_application)->OnDisplayGeometryChanged(display_rotation, width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onGlSurfaceDrawFrame(JNIEnv *env, jclass clazz,
                                                               jlong native_application) {
    Native(native_application)->OnDrawFrame();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onGlSurfaceCreated(JNIEnv *env, jclass clazz,
                                                             jlong native_application) {
    Native(native_application)->OnSurfaceCreated();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onTouched(JNIEnv *env, jclass clazz,
                                                    jlong native_application, jfloat x, jfloat y,jboolean longtap) {
    Native(native_application)->OnTouched(x, y, longtap);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_destroyNativeApplication(JNIEnv *env, jclass clazz,
                                                                   jlong native_application) {
    // must release jboject
    if (g_ar_activity_) {
        env->DeleteGlobalRef(g_ar_activity_);
        g_ar_activity_ = nullptr;
    }
    delete Native(native_application);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_setRotateRadius(JNIEnv *env, jclass clazz,
                                                          jlong native_application,
                                                          jfloat rotate_radius) {
    Native(native_application)->set_rotation_radius(rotate_radius);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_setScaleFactor(JNIEnv *env, jclass clazz,
                                                         jlong native_application,
                                                         jfloat scal_factor) {
    Native(native_application)->SetScaleFactor(scal_factor);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_liblarkar_JniInterface_onPause(JNIEnv *env, jclass clazz, jlong native_application) {
    Native(native_application)->OnPause();
}
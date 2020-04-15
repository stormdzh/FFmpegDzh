//
// Created by tal on 2020-04-15.
//
#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring
JNICALL
//com.stormdzh.libaudio.util.TestJni.stringFromJni
Java_com_stormdzh_libaudio_util_TestJni_stringFromJni(JNIEnv *env,
                                                      jobject) {

    std::string hello = "hello jni";

    return env->NewStringUTF(hello.c_str());
}

//
// Created by dzh on 2020-04-17.
//

#ifndef FFMPEGDZH_DZGCALLJAVA_H
#define FFMPEGDZH_DZGCALLJAVA_H


#include <jni.h>
#include <cwchar>

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class WlCallJava {

public:
    JavaVM *javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmid_prepare;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_pcmtoaac;
    jmethodID jmid_pcminfo;
    jmethodID jmid_renderyuv;
    jmethodID jmid_supportMediaCodec;
    jmethodID jmid_initMediaCodec;
    jmethodID jmid_decodeavpackt;

public:
    WlCallJava(JavaVM *javaVm, JNIEnv *jniEnv, jobject *jobj);

    ~WlCallJava();

    //准备好了
    void onCallPrepare(int type);

    //加载中
    void onCallLoad(int type, bool load);

    //播放时间
    void onCallTimeInfo(int type, int curTime, int duration);

    //异常回调
    void onCallError(int type, int code, char *msg);

    //播放完成回调
    void onCallComplete(int type);

    //返回pcm数据，保存为aac
    void onCallPcmToAAc(int type, int size, void *buffer);

    //返回pcm数据，硬编码使用
    void onCallPcmInfo(int type, void *buffer, int size);

    //返回yuv数据，给Surfaceview渲染
    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

    //判断改解码器是否支持硬解码
    bool onCallIsSupportMediaCode(const char *ffcodecname);

    //调用java层初始化硬解码
    void onCallInitMediaCodec(const char *mine, int width, int heght, int csd0_size, uint8_t *csd_0,
                              int csd1_size, uint8_t *csd_1);

    //返回需要解码的数据包
    void onCallDecodeAvpackt(int datasize, uint8_t *avpacktdata);


};


#endif //FFMPEGDZH_DZGCALLJAVA_H

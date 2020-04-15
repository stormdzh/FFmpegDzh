package com.stormdzh.libaudio.util;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-15 15:29
 */
public class TestJni {

    static {
        System.loadLibrary("native-lib");
    }

    //测试获取字符串
    private native String stringFromJni();

    //获取ffmpeg的版本号
    private native String testFFmpeg();

    //创建线程
    public native void normalThread();

    //停止线程
    public native void stopNormalThread();

    public String getStringFromJNI() {

        return stringFromJni();
    }

    public String getTestFFmpeg() {

        return testFFmpeg();
    }


}

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

    private native String stringFromJni();

    private native String testFFmpeg();

    public String getStringFromJNI() {

        return stringFromJni();
    }

    public String getTestFFmpeg() {

        return testFFmpeg();
    }

}

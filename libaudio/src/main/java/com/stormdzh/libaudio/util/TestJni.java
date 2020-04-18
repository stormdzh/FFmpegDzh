package com.stormdzh.libaudio.util;

import android.text.TextUtils;
import android.util.Log;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-15 15:29
 */
public class TestJni {

    private static final String TAG = "MFFMPEG";

    static {
        System.loadLibrary("native-lib");
    }

    //测试获取字符串
    public native String stringFromJni();

    //获取ffmpeg的版本号
    public native String testFFmpeg();

    //创建线程
    public native void normalThread();

    //停止线程
    public native void stopNormalThread();

    //测试生产者、消费者
    public native void mutexThread();

    //c++调用java
    private OnErrerListener onErrerListener;

    public void setOnErrerListener(OnErrerListener onErrerListener) {
        this.onErrerListener = onErrerListener;
    }

    public void onError(int code, String msg) {
        if (onErrerListener != null) {
            onErrerListener.onError(code, msg);
        }
    }

    public interface OnErrerListener {
        void onError(int code, String msg);
    }

    //c++调用java
    public native void callbackFromC();


    public native void playPcm(String pcmPath);


    //------------------------------------

    private PlayerPrepareListener mPlayerPrepareListener;

    public void setPlayerPrepareListener(PlayerPrepareListener playerPrepareListener) {
        this.mPlayerPrepareListener = playerPrepareListener;
    }

    public void mprepare() {

        if (TextUtils.isEmpty(mSource)) {
            Log.i(TAG, "播放地址为空");
            return;
        }else{
            Log.i(TAG, "可以播放");
        }
        prepare(mSource);
    }

    public void mStart(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                start();
            }
        }).start();
    }

    private String mSource;

    public void setSource(String source) {
        this.mSource = source;
    }

    public native void prepare(String mSource);

    //c++ 回调java
    public void onCallPrepare() {
        if (mPlayerPrepareListener != null) {
            mPlayerPrepareListener.onPrepared();
        }
    }

    public native void start();

    //-----------------------------------

}

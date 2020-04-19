package com.stormdzh.libaudio.util;

import android.text.TextUtils;
import android.util.Log;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-15 15:29
 */
public class TestJni {

    private static final String TAG = "MFFMPEGJNI";

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
    private OnPlayEventListener mOnPlayEventListener;

    public void setPlayerPrepareListener(PlayerPrepareListener playerPrepareListener) {
        this.mPlayerPrepareListener = playerPrepareListener;
    }

    public void setOnPlayEventListener(OnPlayEventListener playEventListener) {
        this.mOnPlayEventListener = playEventListener;
    }

    public void mprepare() {

        if (TextUtils.isEmpty(mSource)) {
            Log.i(TAG, "播放地址为空");
            return;
        } else {
            Log.i(TAG, "可以播放");
        }
//        onCallLoad(true);
        prepare(mSource);
    }

    public void mStart() {
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


    public void onCallLoad(boolean load) {
        Log.i(TAG, "onCallLoad load:" + load);
        if (load) {
            Log.i(TAG, "加载中。。。。。。。");
        } else {
            Log.i(TAG, "播放中。。。。。。。");
        }
    }


    public native void pause();

    public native void resume();

    //c++ 回调音频进度
    public void onCallTimeInfo(int curtime, int duration) {
        Log.i(TAG, "onCallTimeInfo duration:" + duration + " curtime:" + curtime);
        if (mOnPlayEventListener != null) {
            mOnPlayEventListener.onProgress(curtime, duration);
        }
    }

    //c++ 错误 回调
    public void onCallError(int code, String msg) {
        //错误停止视频，释放资源
        onCallLoad(false);
        nstop();
        if (mOnPlayEventListener != null) {
            mOnPlayEventListener.onError(code, msg);
        }
    }

    public void stop() {
        nstop();
    }

    public native void nstop();

    //-----------------------------------

}

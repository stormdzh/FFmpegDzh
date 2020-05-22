package com.stormdzh.libaudio.util;

/**
 * @Description: 播放状态的监听
 * @Author: dzh
 * @CreateDate: 2020-04-19 01:37
 */
public interface OnPlayEventListener {

    void onStart();

    void onProgress(int curtime, int duration);

    void onPuase();

    void onStop();

    void onComplete();

    void onDestry();

    void onError(int code, String msg);

}

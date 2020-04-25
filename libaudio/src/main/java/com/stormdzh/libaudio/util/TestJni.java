package com.stormdzh.libaudio.util;

import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.stormdzh.libaudio.util.opengl.WLGLSufurfaceView;
import com.stormdzh.libaudio.util.opengl.WlVideoRender;

import java.io.File;

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

    //c++ 错误 回调
    public void onCallComplete() {
        //错误停止视频，释放资源
        nstop();
        if (mediaCodecVideo != null) {
            mediaCodecVideo.release();
        }
        if (mOnPlayEventListener != null) {
            mOnPlayEventListener.onComplete();
        }
    }

    public void stop() {
        nstop();
    }

    public native void nstop();

    //seek到秒
    public native void seekto(int msc);

    //需要在prepare后才能设置
    public native void setVolume(int percent);

    public native int getVolume();


    public void volumeLeft() {
        setMute(1);
    }

    public void volumeRight() {
        setMute(0);
    }

    public void volumeLeftRight() {
        setMute(2);
    }

    public native void setMute(int type);

    public native void setPitch(double newPitch);

    public native void setTempo(double newTempo);


    public native int getSampleRate();

    //-----------------------------------

    private native void startRecordPcm(boolean isRecordPcm);

    MediaCodecUtil mMediaCodecUtil = null;
    private boolean isStartRecordPcm = false;

    public void stopRecord() {
        if (!isStartRecordPcm) {
            return;
        }
        startRecordPcm(false);
        Log.i("DDD", "encodecPcmToAAc  停止");
        isStartRecordPcm = false;
        if (mMediaCodecUtil == null) {
            mMediaCodecUtil.releaseMedicacodec();
        }
    }

    public void startRecord(File outFile) {
        if (isStartRecordPcm)
            return;
        isStartRecordPcm = true;
        if (mMediaCodecUtil == null) {
            mMediaCodecUtil = new MediaCodecUtil();
        }
        Log.i("DDD", "encodecPcmToAAc  getSampleRate:" + getSampleRate());
        if (getSampleRate() > 0) {
            mMediaCodecUtil.initMediacodec(getSampleRate(), outFile);
            Log.i("DDD", "encodecPcmToAAc  开始");
            startRecordPcm(true);
        }
    }

    public void continueRecord() {
        startRecordPcm(true);
    }

    public void paauseRecord() {
        startRecordPcm(false);
    }

    //C++回调的方法
    private void encodecPcmToAAc(final int size, final byte[] buffer) {
//        Log.i("DDD","encodecPcmToAAc  size:"+size);
        if (mMediaCodecUtil != null) {
            mMediaCodecUtil.encodecPcmToAAc(size, buffer);

        }
    }

    //------------------音频裁剪-------------------------
    public void mCutAudioPlay(int satrtTiem, int endTime, boolean showPcmData) {
        if (cutAudioPlay(satrtTiem, endTime, showPcmData)) {
            start();
        }
    }

    public void onCallPcmInfo(byte[] buffer, int size) {
        Log.i("DDD", "获取到pcm数据 size：" + size);
    }

    private native boolean cutAudioPlay(int satrtTiem, int endTime, boolean showPcmData);


    //------------------视频播放器-------------------------

    private WLGLSufurfaceView mWLGLSufurfaceView;
    private Surface mSurface;

    public void setGLSufurfaceView(WLGLSufurfaceView glSufurfaceView) {
        this.mWLGLSufurfaceView = glSufurfaceView;

        if (mediaCodecVideo == null) {
            mediaCodecVideo = new MediaCodecVideo();
        }
        mediaCodecVideo.setmWLGLSufurfaceView(mWLGLSufurfaceView);

        if(mWLGLSufurfaceView!=null) {
            mWLGLSufurfaceView.getVideoRender().setOnSurfaceCreatedListener(new WlVideoRender.OnSurfaceCreatedListener() {
                @Override
                public void onSurfaceCreated(Surface surface) {
                    if (mSurface == null) {
                        mSurface = surface;
                    }
                    mediaCodecVideo.setmSurface(mSurface);
                    Log.i("mediacodec_video", "onSurfaceCreated");
                }
            });
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {

        Log.i("DDD", "收到ffmpeg回调过来yuv数据 onCallRenderYUV");
        if (mWLGLSufurfaceView != null) {
            mWLGLSufurfaceView.getVideoRender().setRenderType(WlVideoRender.RENDER_YUV);
            mWLGLSufurfaceView.setYUVData(width, height, y, u, v);
        }
    }

    //通过编码名称判断，是否支持硬解码
    public boolean onCallIsSupportMediaCode(String ffCodeNama) {
        Log.i("mediacodec_video","视频编码名称："+ffCodeNama);
        return WlVideoSupportUitl.isSupportCodec(ffCodeNama)&&false;
    }

    MediaCodecVideo mediaCodecVideo;

    public void initMediaCodec(String codecName, int width, int height, byte[] csd_0, byte[] csd_1) {

        if(mediaCodecVideo!=null) {
            mediaCodecVideo.initMediaCodec(codecName, width, height, csd_0, csd_1);
        }
        Log.i("mediacodec_video", "init mediacodec");
    }

    public void decodeAvPackt(int size, byte[] packtData) {

        if (mediaCodecVideo != null) {
            mediaCodecVideo.decodeAVPacket(size, packtData);
        }
    }


}

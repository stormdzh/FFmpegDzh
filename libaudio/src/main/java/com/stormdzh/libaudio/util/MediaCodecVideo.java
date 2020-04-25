package com.stormdzh.libaudio.util;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import com.stormdzh.libaudio.util.opengl.WLGLSufurfaceView;
import com.stormdzh.libaudio.util.opengl.WlVideoRender;

import java.nio.ByteBuffer;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-25 14:23
 */
public class MediaCodecVideo {

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private Surface mSurface;
    private MediaCodec.BufferInfo info;


    /**
     * 初始化MediaCodec
     *
     * @param codecName
     * @param width
     * @param height
     * @param csd_0
     * @param csd_1
     */
    public void initMediaCodec(String codecName, int width, int height, byte[] csd_0, byte[] csd_1) {
        Log.d("mediacodec_video", "初始化mediacodec");
        if (mSurface != null) {
            try {
                mWLGLSufurfaceView.getVideoRender().setRenderType(WlVideoRender.RENDER_MEDIACODEC);
                String mime = WlVideoSupportUitl.findVideoCodecName(codecName);
                mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
                mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
                Log.d("mediacodec_video", mediaFormat.toString());
                mediaCodec = MediaCodec.createDecoderByType(mime);

                info = new MediaCodec.BufferInfo();
                mediaCodec.configure(mediaFormat, mSurface, null, 0);
                mediaCodec.start();
                Log.d("mediacodec_video", "初始化mediacodec成功");
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            Log.d("mediacodec_video", "初始化mediacodec mSurface ==null");
//            if (wlOnErrorListener != null) {
//                wlOnErrorListener.onError(2001, "mSurface is null");
//            }
        }
    }

    public void decodeAVPacket(int datasize, byte[] data) {
        if (mSurface != null && datasize > 0 && data != null && mediaCodec != null) {
            int intputBufferIndex = mediaCodec.dequeueInputBuffer(10);
            if (intputBufferIndex >= 0) {
                ByteBuffer byteBuffer = mediaCodec.getInputBuffers()[intputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(data);
                mediaCodec.queueInputBuffer(intputBufferIndex, 0, datasize, 0, 0);
            }
            int outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
            while (outputBufferIndex >= 0) {
                mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
            }
        }
    }

    public void setmSurface(Surface surface) {
        if (this.mSurface == null) {
//            Log.i("mediacodec_video", "setmSurface 参数为空吗：" + (surface == null));
            this.mSurface = surface;
        }
    }

    private WLGLSufurfaceView mWLGLSufurfaceView;

    public void setmWLGLSufurfaceView(WLGLSufurfaceView sufurfaceView) {
        if (mWLGLSufurfaceView == null) {
            Log.i("mediacodec_video", "setmWLGLSufurfaceView");
            this.mWLGLSufurfaceView = sufurfaceView;
        }
    }


    public void release() {
        if (mediaCodec != null) {
            Log.i("mediacodec_video", "release");
            mediaCodec.flush();
            mediaCodec.stop();
            mediaCodec.release();
            mediaCodec = null;
            mediaFormat = null;
            info = null;
        }
    }
}

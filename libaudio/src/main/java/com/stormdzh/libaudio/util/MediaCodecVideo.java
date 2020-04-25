package com.stormdzh.libaudio.util;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-25 14:23
 */
public class MediaCodecVideo {

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private Surface surface;
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
        if (surface != null) {
            try {
                String mime = WlVideoSupportUitl.findVideoCodecName(codecName);
                mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
                mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
                Log.d("initMediaCodec", mediaFormat.toString());
                mediaCodec = MediaCodec.createDecoderByType(mime);

                mediaCodec.configure(mediaFormat, surface, null, 0);
                mediaCodec.start();

            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
//            if(wlOnErrorListener != null)
//            {
//                wlOnErrorListener.onError(2001, "surface is null");
//            }
        }
    }

    public void decodeAVPacket(int datasize, byte[] data) {
        Log.i("decodeAVPacket", "decodeAVPacket size:" + datasize);
        if (surface != null && mediaCodec != null && datasize > 0 && data != null) {
            int intputBufferIndex = mediaCodec.dequeueInputBuffer(10);
            if (intputBufferIndex >= 0) {
                ByteBuffer byteBuffer = mediaCodec.getOutputBuffers()[intputBufferIndex];
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
}

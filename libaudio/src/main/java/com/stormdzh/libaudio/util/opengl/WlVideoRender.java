package com.stormdzh.libaudio.util.opengl;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Surface;

import com.stormdzh.libaudio.R;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-22 10:17
 */
public class WlVideoRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    public static final int RENDER_YUV=0;
    public static final int RENDER_MEDIACODEC=1;
    private int renderType=RENDER_YUV;
    private Context mContext;
    private int program_yuv;
    private int avPosition_yuv;
    private int afPosition_yuv;
    private int sTexure;
    private int texureId;

    private int sampler_y;
    private int sampler_u;
    private int sampler_v;

    int[] textureid_yuv;

    int width_yuv;
    int height_yuv;

    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;


    //mediacodec
    private int program_mediacodec;
    private int avPosition_mediacodec;
    private int afPosition_mediacodec;
    private int samplerOES_mediacodec;
    private int textureId_mediacodec;
    private SurfaceTexture surfaceTexture;
    private Surface surface;
    private OnSurfaceCreatedListener mOnSurfaceCreatedListener;
    private OnRenderListener mOnRenderListener;


    //    顶点坐标
    private final float[] vertexData = {

            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f


    };

    //    纹理坐标
    private final float[] textureData = {

//            正常展示
            0f, 1f,
            1f, 1f,
            0, 0,
            1f, 0


    };

    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;

    public WlVideoRender(Context context) {
        this.mContext = context;
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        vertexBuffer.position(0);


        textureBuffer = ByteBuffer.allocateDirect(textureData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(textureData);
        textureBuffer.position(0);

    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        intRenderYUV();
        initRenderMideaCodec();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        //设置空间宽高位置
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        //清屏
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        //清屏为红色
        GLES20.glClearColor(0, 0, 0, 1.0f);

        if(renderType==RENDER_YUV) {
            renderYUV();
        }else{
            renderMediaCodec();
        }

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

    }

    private void intRenderYUV() {

        String vertexSource = WlShaderUtil.readRawText(mContext, R.raw.vertex_video_shader);
        String fragmentSource = WlShaderUtil.readRawText(mContext, R.raw.fragment_yuv_shader);
        program_yuv = WlShaderUtil.creteProgram(vertexSource, fragmentSource);

        avPosition_yuv = GLES20.glGetAttribLocation(program_yuv, "av_Position");
        afPosition_yuv = GLES20.glGetAttribLocation(program_yuv, "af_Position");


        sampler_y = GLES20.glGetUniformLocation(program_yuv, "sampler_y");
        sampler_u = GLES20.glGetUniformLocation(program_yuv, "sampler_u");
        sampler_v = GLES20.glGetUniformLocation(program_yuv, "sampler_v");


        textureid_yuv = new int[3];
        GLES20.glGenTextures(3, textureid_yuv, 0);

        for (int i = 0; i < 3; i++) {

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[i]);
            //设置环绕方式
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            //过滤方式
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        }

    }

    public void setYUVRenderDataa(int width, int height, byte[] y, byte[] u, byte[] v) {
        this.width_yuv = width;
        this.height_yuv = height;
        this.y = ByteBuffer.wrap(y);
        this.u = ByteBuffer.wrap(u);
        this.v = ByteBuffer.wrap(v);

    }

    public void renderYUV() {

        if (width_yuv < 0 || height_yuv < 0 || y == null || u == null || v == null) return;

        GLES20.glUseProgram(program_yuv);

        GLES20.glEnableVertexAttribArray(avPosition_yuv);
        GLES20.glVertexAttribPointer(avPosition_yuv, 2, GLES20.GL_FLOAT, false, 2 * 4, vertexBuffer);

        GLES20.glEnableVertexAttribArray(afPosition_yuv);
        GLES20.glVertexAttribPointer(afPosition_yuv, 2, GLES20.GL_FLOAT, false, 2 * 4, textureBuffer);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[0]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv, height_yuv, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[1]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, u);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[2]);
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, v);

//        使用纹理
        GLES20.glUniform1i(sampler_y, 0);
        GLES20.glUniform1i(sampler_u, 1);
        GLES20.glUniform1i(sampler_v, 2);

        y.clear();
        u.clear();
        v.clear();
        y = null;
        u = null;
        v = null;
        //这句代码不要写在这，播放中可能会有黑色闪屏
//        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    private void initRenderMideaCodec(){
        String vertexSource = WlShaderUtil.readRawText(mContext, R.raw.vertex_video_shader);
        String fragmentSource = WlShaderUtil.readRawText(mContext, R.raw.fragment_mediacodec_shader);
        program_mediacodec = WlShaderUtil.creteProgram(vertexSource, fragmentSource);

        avPosition_mediacodec = GLES20.glGetAttribLocation(program_mediacodec, "av_Position");
        afPosition_mediacodec = GLES20.glGetAttribLocation(program_mediacodec, "af_Position");
        samplerOES_mediacodec = GLES20.glGetUniformLocation(program_mediacodec, "sTexture");

        int[] textureids = new int[1];
        GLES20.glGenTextures(1, textureids, 0);
        textureId_mediacodec = textureids[0];

        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        surfaceTexture = new SurfaceTexture(textureId_mediacodec);
        surface = new Surface(surfaceTexture);
        surfaceTexture.setOnFrameAvailableListener(this);

        if(mOnSurfaceCreatedListener!=null){
            mOnSurfaceCreatedListener.onSurfaceCreated(surface);
        }
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

//        Log.i("mediacodec_video","onFrameAvailable");
        if(mOnRenderListener!=null){
            mOnRenderListener.onRender();
        }
    }

    public interface OnSurfaceCreatedListener{
        void onSurfaceCreated(Surface surface);
    }

    public void setOnSurfaceCreatedListener(OnSurfaceCreatedListener listener){
        this.mOnSurfaceCreatedListener=listener;
    }

    public void setRenderType(int type){
        this.renderType=type;
    }

    private void renderMediaCodec() {
        surfaceTexture.updateTexImage();
        GLES20.glUseProgram(program_mediacodec);

        GLES20.glEnableVertexAttribArray(avPosition_mediacodec);
        GLES20.glVertexAttribPointer(avPosition_mediacodec, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);

        GLES20.glEnableVertexAttribArray(afPosition_mediacodec);
        GLES20.glVertexAttribPointer(afPosition_mediacodec, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId_mediacodec);
        GLES20.glUniform1i(samplerOES_mediacodec, 0);

    }

    public interface OnRenderListener{
        void onRender();
    }

    public void setOnRenderListener(OnRenderListener listener){
        this.mOnRenderListener=listener;
    }


}

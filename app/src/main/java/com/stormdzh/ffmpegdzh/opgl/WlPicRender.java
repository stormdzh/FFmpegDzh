package com.stormdzh.ffmpegdzh.opgl;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;

import com.stormdzh.ffmpegdzh.R;

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
public class WlPicRender implements GLSurfaceView.Renderer {

    private Context mContext;
    private int program;
    private int avPosition;
    private int afPosition;
    private int sTexure;
    private int texureId;

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
//            0f, 1f,
//            1f, 1f,
//            0, 0,
//            1f, 0
//            图片到过来 ,把当前坐标活成对面的顶点坐标
            1, 0,
            0, 0,
            1, 1,
            0, 1


    };

    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;

    public WlPicRender(Context context) {
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

        String vertexSource = WlShaderUtil.readRawText(mContext, R.raw.vertex_pic_shader);
        String fragmentSource = WlShaderUtil.readRawText(mContext, R.raw.fragment_pic_shader);
        program = WlShaderUtil.creteProgram(vertexSource, fragmentSource);
        if (program > 0) {
            avPosition = GLES20.glGetAttribLocation(program, "av_Position");
            afPosition = GLES20.glGetAttribLocation(program, "af_Position");
            sTexure = GLES20.glGetUniformLocation(program, "sTexture");

            //创建纹理
            int[] textureIds = new int[1];
            GLES20.glGenTextures(1, textureIds, 0);
            if (textureIds[0] == 0) {
                return;
            }
            texureId = textureIds[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texureId);

            //设置环绕方式
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            //过滤方式
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

            Bitmap bitmap = BitmapFactory.decodeResource(mContext.getResources(), R.mipmap.og);
            if (bitmap == null) return;
            //设置图片
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

            if (bitmap != null) {
                bitmap.recycle();
                bitmap = null;
            }
        }
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
        GLES20.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        GLES20.glUseProgram(program);

        GLES20.glEnableVertexAttribArray(avPosition);
        GLES20.glVertexAttribPointer(avPosition, 2, GLES20.GL_FLOAT, false, 2 * 4, vertexBuffer);
//        复用前面三角形的两个顶点 绘制过程v1 v2 v3 ; v2 v3 v4
//        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glEnableVertexAttribArray(afPosition);
        GLES20.glVertexAttribPointer(afPosition, 2, GLES20.GL_FLOAT, false, 2 * 4, textureBuffer);
//        复用前面三角形的两个顶点 绘制过程v1 v2 v3 ; v2 v3 v4
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }
}

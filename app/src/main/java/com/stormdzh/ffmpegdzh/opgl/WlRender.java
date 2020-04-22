package com.stormdzh.ffmpegdzh.opgl;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

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
public class WlRender implements GLSurfaceView.Renderer {

    private Context mContext;
    private int program;
    private int avPosition;
    private int afColor;


    private final float[] vertexData = {

            -1f, 0,
            0, -1f,
            0, 1f,


            0, 1f,
            0, -1f,
            1f, 0

    };

    private FloatBuffer vertexBuffer;

    public WlRender(Context context) {
        this.mContext = context;
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);

        vertexBuffer.position(0);
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {

        String vertexSource = WlShaderUtil.readRawText(mContext, R.raw.vertex_shader);
        String fragmentSource = WlShaderUtil.readRawText(mContext, R.raw.fragment_shader);
        program = WlShaderUtil.creteProgram(vertexSource, fragmentSource);
        if (program > 0) {
            avPosition = GLES20.glGetAttribLocation(program, "av_Position");
            afColor = GLES20.glGetUniformLocation(program, "af_Color");
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

        GLES20.glUniform4f(afColor, 1f, 0, 0, 1);

        GLES20.glEnableVertexAttribArray(avPosition);
        GLES20.glVertexAttribPointer(avPosition, 2, GLES20.GL_FLOAT, false, 2 * 4, vertexBuffer);
//        一个三角形
//        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 3);
//        两个三角形
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 6);
    }
}

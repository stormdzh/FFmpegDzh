package com.stormdzh.libaudio.util.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * @Description: 视频渲染Surfaceview
 * @Author: dzh
 * @CreateDate: 2020-04-22 10:16
 */
public class DzhGLSufurfaceView extends GLSurfaceView {

    private DzhVideoRender wlVideoRender;

    public DzhGLSufurfaceView(Context context) {
        this(context, null);
    }

    public DzhGLSufurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        init();
    }

    private void init() {
        setEGLContextClientVersion(2);
        //渲染三角形
//        setRenderer(new WlRender(getContext()));
        //渲染图片
        wlVideoRender = new DzhVideoRender(getContext());
        setRenderer(wlVideoRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        //硬解码刷新数据
        wlVideoRender.setOnRenderListener(new DzhVideoRender.OnRenderListener() {
            @Override
            public void onRender() {
                requestRender();
            }
        });
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (wlVideoRender == null) return;
        wlVideoRender.setYUVRenderDataa(width, height, y, u, v);
        requestRender();
    }

    public DzhVideoRender getVideoRender() {
        return wlVideoRender;
    }
}

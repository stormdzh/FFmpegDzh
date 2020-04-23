package com.stormdzh.libaudio.util.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-22 10:16
 */
public class WLGLSufurfaceView extends GLSurfaceView {

    private WlVideoRender wlVideoRender;

    public WLGLSufurfaceView(Context context) {
        this(context, null);
    }

    public WLGLSufurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        init();
    }

    private void init() {
        setEGLContextClientVersion(2);
        //渲染三角形
//        setRenderer(new WlRender(getContext()));
        //渲染图片
        wlVideoRender = new WlVideoRender(getContext());
        setRenderer(wlVideoRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (wlVideoRender == null) return;
        wlVideoRender.setYUVRenderDataa(width, height, y, u, v);
        requestRender();
    }
}

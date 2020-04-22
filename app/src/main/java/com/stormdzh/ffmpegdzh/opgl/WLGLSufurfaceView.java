package com.stormdzh.ffmpegdzh.opgl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-22 10:16
 */
public class WLGLSufurfaceView extends GLSurfaceView {
    public WLGLSufurfaceView(Context context) {
        this(context,null);
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
        setRenderer(new WlPicRender(getContext()));
    }
}

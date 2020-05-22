package com.stormdzh.ffmpegdzh.util;

import android.content.Context;

/**
 * @Description: 工具类
 * @Author: dzh
 * @CreateDate: 2020-05-22 13:51
 */
public class TimeUtil {

    public static String getPlayerDate(Integer date) {
        int h = date / 3600;
        int m = (date % 3600) / 60;
        int s = (date % 3600) % 60;
        return h + ":" + m + ":" + s + ":";
    }

    /**
     * 根据手机的分辨率从 dp 的单位 转成为 px(像素)
     */
    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }


    /**
     * 根据手机的分辨率从 px(像素) 的单位 转成为 dp
     */
    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }
}

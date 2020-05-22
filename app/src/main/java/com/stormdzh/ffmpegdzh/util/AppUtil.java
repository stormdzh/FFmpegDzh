package com.stormdzh.ffmpegdzh.util;

import android.content.Context;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.util.DisplayMetrics;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-05-22 18:51
 */
public class AppUtil {



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

    /**
     * 获取屏幕宽度
     */
    public static int getScreenW(Context aty) {
        DisplayMetrics dm = new DisplayMetrics();
        dm = aty.getResources().getDisplayMetrics();
        int w = dm.widthPixels;
        // int w = aty.getWindowManager().getDefaultDisplay().getWidth();
        return w;
    }

    /**
     * 获取屏幕高度
     * 屏幕有效的高度
     * 就是:DecorView的高度去掉导航栏的高度.
     * 这个高度不管你有没有隐藏导航栏, 这个值都不会改变
     * 这个在有些手机上获取的不包括底部的导航栏例如华为h60-l02 4.4.2
     * 有些手机包含底部的高度例如meizu mx4
     */
    public static int getScreenH(Context aty) {
        DisplayMetrics dm = new DisplayMetrics();
        dm = aty.getResources().getDisplayMetrics();
        int h = dm.heightPixels;
        // int h = aty.getWindowManager().getDefaultDisplay().getHeight();
        return h;
    }



    public static void getList() {

        List<HashMap<String, String>> datas = new ArrayList<HashMap<String, String>>();
        int numCodecs = MediaCodecList.getCodecCount();

        HashMap<String, String> map;

        for (int i = 0; i < numCodecs; i++) {

            MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);

            map = new HashMap<String, String>();

            if (!codecInfo.isEncoder()) {

                continue;

            }

            map.put("decoderName", codecInfo.getName());

            String[] types = codecInfo.getSupportedTypes();

            for (int j = 0; j < types.length; j++) {

                if (map.containsValue(types[j])) {

                    continue;

                } else {

                    map.put("decoderType", types[j]);

                }

            }

            datas.add(map);
        }

        int size = datas.size();
        for(int i=0;i<size;i++){
            HashMap<String, String> stringStringHashMap = datas.get(i);
            Log.i("supportCodec",stringStringHashMap.get("decoderName")+" :  "+stringStringHashMap.get("decoderName"));

        }
    }


}

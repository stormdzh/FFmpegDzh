package com.stormdzh.libaudio.util;

import android.media.MediaCodecList;

import java.util.HashMap;
import java.util.Map;

/**
 * @description 判断是否支持硬解码
 * @author: dzh
 * @CreateDate: ${DATE} ${TIME}
 */
public class DzhVideoSupportUitl {

    private static Map<String, String> codecMap = new HashMap<>();

    static {
        codecMap.put("h264", "video/avc");
    }

    public static String findVideoCodecName(String ffcodename) {
        if (codecMap.containsKey(ffcodename)) {
            return codecMap.get(ffcodename);
        }
        return "";
    }

    /**
     * 通过编码器的名称判断是否支持
     *
     * @param ffcodecname 编码器名称
     * @return true：支持，false 不支持
     */
    public static boolean isSupportCodec(String ffcodecname) {
        boolean supportvideo = false;
        int count = MediaCodecList.getCodecCount();
        for (int i = 0; i < count; i++) {
            String[] tyeps = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for (int j = 0; j < tyeps.length; j++) {
                if (tyeps[j].equals(findVideoCodecName(ffcodecname))) {
                    supportvideo = true;
                    break;
                }
            }
            if (supportvideo) {
                break;
            }
        }
        return supportvideo;
    }
}

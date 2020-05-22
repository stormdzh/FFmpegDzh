package com.stormdzh.ffmpegdzh.util;

import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
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

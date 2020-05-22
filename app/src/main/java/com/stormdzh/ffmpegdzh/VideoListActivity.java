package com.stormdzh.ffmpegdzh;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.stormdzh.ffmpegdzh.adapter.VideoListAdapter;
import com.stormdzh.ffmpegdzh.entity.VideoListEntity;
import com.stormdzh.ffmpegdzh.util.AppUtil;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * @Description: 视频播放列表
 * @Author: dzh
 * @CreateDate: 2020-05-22 10:28
 */
public class VideoListActivity extends Activity {

    private RecyclerView mRecyclerView;
    private List<VideoListEntity> mVideoList;
    private VideoListAdapter mVideoListAdapter;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_video_list);

        mRecyclerView = findViewById(R.id.mRecyclerView);

        setVideoListData();

        AppUtil.getList();
    }

    private void setVideoListData() {

        mRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        initData();
        mVideoListAdapter = new VideoListAdapter(this, mVideoList);
        mRecyclerView.setAdapter(mVideoListAdapter);
    }

    private void initData() {
        mVideoList = new ArrayList<>();

        mVideoList.add(new VideoListEntity("本地视频 h264",new File(Environment.getExternalStorageDirectory(), "testmv.mp4").getAbsolutePath()));
        mVideoList.add(new VideoListEntity("本地视频 h265",new File(Environment.getExternalStorageDirectory(), "video-h265.mkv").getAbsolutePath()));
        mVideoList.add(new VideoListEntity("CCTV1","http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8"));
        mVideoList.add(new VideoListEntity("外MTV HD德国","http://de1se01.v2beat.live/playlist.m3u8"));
        mVideoList.add(new VideoListEntity("北京卫视H","http://ivi.bupt.edu.cn/hls/btv1hd.m3u8"));
        mVideoList.add(new VideoListEntity("湖南卫视HD", "http://ivi.bupt.edu.cn/hls/hunanhd.m3u8"));
        mVideoList.add(new VideoListEntity("東方卫视HD","http://ivi.bupt.edu.cn/hls/dfhd.m3u8"));
        mVideoList.add(new VideoListEntity("江蘇卫视HD","http://ivi.bupt.edu.cn/hls/jshd.m3u8"));
        mVideoList.add(new VideoListEntity("浙江卫视HD","http://ivi.bupt.edu.cn/hls/zjhd.m3u8"));
        mVideoList.add(new VideoListEntity("北京卫视HD","http://ivi.bupt.edu.cn/hls/btv1hd.m3u8"));
        mVideoList.add(new VideoListEntity("安徽卫视HD","http://ivi.bupt.edu.cn/hls/ahhd.m3u8"));
        mVideoList.add(new VideoListEntity("黑龙江卫视HD","http://ivi.bupt.edu.cn/hls/hljhd.m3u8"));
        mVideoList.add(new VideoListEntity("深圳卫视HD","http://ivi.bupt.edu.cn/hls/szhd.m3u8"));
        mVideoList.add(new VideoListEntity("广东卫视HD","http://ivi.bupt.edu.cn/hls/gdhd.m3u8"));
        mVideoList.add(new VideoListEntity("天津卫视HD","http://ivi.bupt.edu.cn/hls/tjhd.m3u8"));
        mVideoList.add(new VideoListEntity("湖北卫视HD","http://ivi.bupt.edu.cn/hls/hbhd.m3u8"));
        mVideoList.add(new VideoListEntity("CCTV1","http://183.207.248.71:80/cntv/live1/CCTV-1/cctv-1"));
        mVideoList.add(new VideoListEntity("CCTV3","http://ivi.bupt.edu.cn/hls/cctv3hd.m3u8"));
        mVideoList.add(new VideoListEntity("CCTV5","http://183.207.248.71:80/cntv/live1/CCTV-5/cctv-5"));
        mVideoList.add(new VideoListEntity("CCTV5+","http://183.207.248.71:80/cntv/live1/CCTV5+/hdcctv05plus"));
        mVideoList.add(new VideoListEntity("CCTV5+ 快","http://ivi.bupt.edu.cn/hls/cctv5phd.m3u8"));
        mVideoList.add(new VideoListEntity("CCTV6 快","http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8"));
        mVideoList.add(new VideoListEntity("CCTV8","http://ivi.bupt.edu.cn/hls/cctv8hd.m3u8"));
        mVideoList.add(new VideoListEntity("CCTV9","http://183.207.248.71:80/cntv/live1/CCTV-9/cctv-news"));
    }
}

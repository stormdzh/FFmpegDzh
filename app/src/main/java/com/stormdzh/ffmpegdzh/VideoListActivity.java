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

        mVideoList.add(new VideoListEntity("CCTV1","http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8"));
        mVideoList.add(new VideoListEntity("本地视频",new File(Environment.getExternalStorageDirectory(), "testmv.mp4").getAbsolutePath()));
    }
}

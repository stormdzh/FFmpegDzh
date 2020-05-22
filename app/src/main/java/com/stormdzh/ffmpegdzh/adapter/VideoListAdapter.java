package com.stormdzh.ffmpegdzh.adapter;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.stormdzh.ffmpegdzh.R;
import com.stormdzh.ffmpegdzh.VideoActivity;
import com.stormdzh.ffmpegdzh.entity.VideoListEntity;

import java.util.List;

/**
 * @Description: 视频列表播放地址
 * @Author: dzh
 * @CreateDate: 2020-05-22 10:38
 */
public class VideoListAdapter extends RecyclerView.Adapter {

    private Context mContext;
    private List<VideoListEntity> mList;

    public VideoListAdapter(Context context, List<VideoListEntity> list) {
        this.mContext = context;
        this.mList = list;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new VideoViewHolder(LayoutInflater.from(mContext).inflate(R.layout.adapter_video_list, parent, false));
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, final int position) {

        VideoViewHolder videoViewHolder= (VideoViewHolder) holder;
        if(position%2==0) {
            videoViewHolder.itemView.setBackgroundColor(Color.parseColor("#FFEFD5"));
        }else{
            videoViewHolder.itemView.setBackgroundColor(Color.parseColor("#FFFACD"));
        }
        videoViewHolder.tvName.setText(mList.get(position).name);
        videoViewHolder.tvName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
              mContext.startActivity(new Intent(mContext, VideoActivity.class).putExtra("vurl",mList.get(position).vurl));
            }
        });
    }

    @Override
    public int getItemCount() {
        return mList.size();
    }

    class VideoViewHolder extends RecyclerView.ViewHolder {

        public TextView tvName;

        public VideoViewHolder(@NonNull View itemView) {
            super(itemView);
            tvName = itemView.findViewById(R.id.tvName);
        }
    }
}

package com.stormdzh.ffmpegdzh.entity;

/**
 * @Description: 视频列表实体类
 * @Author: dzh
 * @CreateDate: 2020-05-22 10:39
 */
public class VideoListEntity {

    public String name; //名称
    public String vurl; //播放地址

    public VideoListEntity(String name, String vurl) {
        this.name = name;
        this.vurl = vurl;
    }
}

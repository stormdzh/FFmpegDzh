package com.stormdzh.ffmpegdzh;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.Nullable;

import com.stormdzh.ffmpegdzh.util.TimeUtil;
import com.stormdzh.libaudio.util.DzhMediaPlayer;
import com.stormdzh.libaudio.util.OnPlayEventListener;
import com.stormdzh.libaudio.util.PlayerPrepareListener;
import com.stormdzh.libaudio.util.opengl.DzhGLSufurfaceView;

/**
 * @Description: 视频播放解码
 * @Author: dzh
 * @CreateDate: 2020-05-22 11:21
 */
public class VideoActivity extends Activity {

    private static final String TAG = "VideoActivity";
    private DzhGLSufurfaceView mGlSufurfaceView;
    private TextView tvDuration;
    private TextView tvCurTime;
    private ImageView ivScale;
    private RelativeLayout videoRoot;

    private DzhMediaPlayer mDzhMediaPlayer;

    private SeekBar mSeekBar;
    public boolean isSeek = false;

    public int mduration = 0;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_video);

        mGlSufurfaceView = findViewById(R.id.glSufurfaceView);
        mSeekBar = findViewById(R.id.mSeekBar);
        tvDuration = findViewById(R.id.tvDuration);
        tvCurTime = findViewById(R.id.tvCurTime);
        ivScale = findViewById(R.id.ivScale);
        videoRoot = findViewById(R.id.videoRoot);


        initPlayer();
        initSeekbar();
        initPlayEvent();
        initScale();

    }

    private void initScale() {

        ivScale.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                //判断当前屏幕方向
                if(getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) {
                    //切换竖屏
                    VideoActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                    setVideoSize(true);
                }else{
                    //切换横屏
                    VideoActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                    setVideoSize(false);
                }

            }
        });
    }

    private void setVideoSize(boolean portrait) {

        RelativeLayout.LayoutParams layoutParams = (RelativeLayout.LayoutParams) videoRoot.getLayoutParams();

        if(portrait){
            layoutParams.height=TimeUtil.dip2px(this,250);
        }else{
            layoutParams.height=RelativeLayout.LayoutParams.MATCH_PARENT;
        }

        videoRoot.setLayoutParams(layoutParams);
    }

    private void initPlayEvent() {
        mDzhMediaPlayer.setOnPlayEventListener(new OnPlayEventListener() {

            @Override
            public void onStart() {

            }

            @Override
            public void onProgress(final int curtime, final int duration) {
                mduration = duration;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        tvCurTime.setText(TimeUtil.getPlayerDate(curtime));
                        tvDuration.setText(TimeUtil.getPlayerDate(duration));
                        double per = ((double) curtime / (double) duration) * 100;
                        if (!isSeek && duration > 0) {
                            mSeekBar.setProgress((int) per);
                        }
                    }
                });
            }

            @Override
            public void onPuase() {

            }

            @Override
            public void onStop() {

            }

            @Override
            public void onComplete() {
                Log.i(TAG, "播放完成：");
            }

            @Override
            public void onDestry() {

            }

            @Override
            public void onError(int code, String msg) {
                Log.i(TAG, "播放错误 code：" + code + "   msg:" + msg);
            }
        });
    }

    private void initSeekbar() {
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean isfromUser) {

                if (isfromUser) {

                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isSeek = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                int progress = seekBar.getProgress();
                if (mDzhMediaPlayer != null) {
                    int seekto = (int) (mduration * ((double) progress / 100));
                    Log.i(TAG, "seek 时间：" + seekto);
                    mDzhMediaPlayer.seekto(seekto);
                }

                isSeek = false;
            }
        });
    }

    private void initPlayer() {
        mDzhMediaPlayer = new DzhMediaPlayer();
        mDzhMediaPlayer.setGLSufurfaceView(mGlSufurfaceView);

        mDzhMediaPlayer.setPlayerPrepareListener(new PlayerPrepareListener() {
            @Override
            public void onPrepared() {
                mDzhMediaPlayer.mStart();
            }
        });

        String vurl = getIntent().getStringExtra("vurl");
        mDzhMediaPlayer.setSource(vurl);
        mDzhMediaPlayer.mprepare();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mGlSufurfaceView != null)
            mGlSufurfaceView.setKeepScreenOn(true);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mGlSufurfaceView != null)
            mGlSufurfaceView.setKeepScreenOn(false);
    }

    @Override
    protected void onDestroy() {

        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        if (mDzhMediaPlayer != null) {
            mDzhMediaPlayer.stop();
        }
        super.onBackPressed();
    }
}

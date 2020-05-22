package com.stormdzh.ffmpegdzh;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.stormdzh.libaudio.util.OnPlayEventListener;
import com.stormdzh.libaudio.util.PlayerPrepareListener;
import com.stormdzh.libaudio.util.DzhMediaPlayer;
import com.stormdzh.libaudio.util.opengl.DzhGLSufurfaceView;

import java.io.File;
import java.util.ArrayList;

public class MainActivity extends Activity implements View.OnClickListener {

    private static final String TAG = "ACTMFFMPEG";

    public int mduration = 0;
    private TextView tvTest;
    private TextView tvVersion;
    private TextView tvProgress;
    private SeekBar mSeekBar;
    private SeekBar mVolume;

    private DzhMediaPlayer mDzhMediaPlayer;

    private DzhGLSufurfaceView mGlSufurfaceView;

    public boolean isSeek = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mDzhMediaPlayer = new DzhMediaPlayer();

        requestPermissions();

        tvTest = findViewById(R.id.tvTest);
        tvVersion = findViewById(R.id.tvVersion);
        tvProgress = findViewById(R.id.tvProgress);
        mSeekBar = findViewById(R.id.mSeekBar);
        mVolume = findViewById(R.id.mVolume);
        mGlSufurfaceView = findViewById(R.id.glSufurfaceView);
        mGlSufurfaceView.setKeepScreenOn(true);
        mDzhMediaPlayer.setGLSufurfaceView(mGlSufurfaceView);

        findViewById(R.id.btnNormalThread).setOnClickListener(this);
        findViewById(R.id.btnStopNormalThread).setOnClickListener(this);
        findViewById(R.id.btnCoustom).setOnClickListener(this);
        findViewById(R.id.btnPlayPcm).setOnClickListener(this);
        findViewById(R.id.btnStart).setOnClickListener(this);
        findViewById(R.id.btnPause).setOnClickListener(this);
        findViewById(R.id.btnResume).setOnClickListener(this);
        findViewById(R.id.btnStop).setOnClickListener(this);
        findViewById(R.id.btnLeft).setOnClickListener(this);
        findViewById(R.id.btnRight).setOnClickListener(this);
        findViewById(R.id.btnLeftRight).setOnClickListener(this);

        findViewById(R.id.btnPitch1).setOnClickListener(this);
        findViewById(R.id.btnPitch15).setOnClickListener(this);
        findViewById(R.id.btnTempo1).setOnClickListener(this);
        findViewById(R.id.btnTempo2).setOnClickListener(this);
        findViewById(R.id.btnStartRecord).setOnClickListener(this);
        findViewById(R.id.btnStopRecord).setOnClickListener(this);
        findViewById(R.id.btnPuaseRecord).setOnClickListener(this);
        findViewById(R.id.btnContinueRecord).setOnClickListener(this);
        findViewById(R.id.btnCutAudioPlay).setOnClickListener(this);
        findViewById(R.id.btnOpenGLES).setOnClickListener(this);


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

        mVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean b) {
                Log.i(TAG, "seek 音量：" + progress);
                mDzhMediaPlayer.setVolume(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });


        tvTest.setText(mDzhMediaPlayer.stringFromJni());
        tvVersion.setText("获取到ffmpeg版本号:" + mDzhMediaPlayer.testFFmpeg());

        mDzhMediaPlayer.setOnPlayEventListener(new OnPlayEventListener() {

            @Override
            public void onStart() {

            }

            @Override
            public void onProgress(final int curtime, final int duration) {
                MainActivity.this.mduration = duration;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        double per = ((double) curtime / (double) duration) * 100;
                        tvProgress.setText("当前播放进度:" + curtime + "/" + duration + "  百分比：" + ((int) per) + "%");
                        if(!isSeek&&duration>0){
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


        //直接跳转到列表
        startActivity(new Intent(this,VideoListActivity.class));

    }

    private void requestPermissions() {
        ArrayList<String> ps = new ArrayList<>();
        int per = ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA);
        if (per != PackageManager.PERMISSION_GRANTED) {
            ps.add(Manifest.permission.CAMERA);
        }
        per = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (per != PackageManager.PERMISSION_GRANTED) {
            ps.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
        per = ContextCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE);
        if (per != PackageManager.PERMISSION_GRANTED) {
            ps.add(Manifest.permission.READ_PHONE_STATE);
        }
        if (!ps.isEmpty()) {
            String[] ps3 = new String[ps.size()];
            ps.toArray(ps3);
            ActivityCompat.requestPermissions(this, ps3, 100);
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnNormalThread:
                mDzhMediaPlayer.normalThread();
                break;
            case R.id.btnStopNormalThread:
                mDzhMediaPlayer.stopNormalThread();
                break;
            case R.id.btnCoustom:
                mDzhMediaPlayer.mutexThread();
                mDzhMediaPlayer.callbackFromC();
                mDzhMediaPlayer.setOnErrerListener(new DzhMediaPlayer.OnErrerListener() {
                    @Override
                    public void onError(int code, String msg) {
                        Log.i("MFFMPEG", "code:" + code + "  msg:" + msg);
                    }
                });
                break;
            //播放pcm
            case R.id.btnPlayPcm:
                File pcmFile = new File(Environment.getExternalStorageDirectory(), "mydream.pcm");
//                File pcmFile = new File(Environment.getExternalStorageDirectory(), "resample.pcm");
                mDzhMediaPlayer.playPcm(pcmFile.getAbsolutePath());
                break;
            case R.id.btnStart:
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "3_test.wav");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "mp3test.mp3");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "bb.mp3");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "study-ffmepg-video.mp4");
                File mp3File = new File(Environment.getExternalStorageDirectory(), "testmv.mp4");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "蔡琴-出塞曲.ape");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "aa.mp3");
                if (!mp3File.exists()) {
                    Log.i(TAG, "文件不存在");
                    return;
                }
                Log.i(TAG, "btnStart");
//                mDzhMediaPlayer.setSource(mp3File.getAbsolutePath());
                mDzhMediaPlayer.setSource("http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8");
//                mDzhMediaPlayer.setSource("https://wp.zp68.com/sub/filestores/2016/07/20/2ad2953e033490cced7ab572564e84b5.mp3");
//                 mDzhMediaPlayer.setSource("http://stormful.oss-cn-shanghai.aliyuncs.com/0era/english/exam/10001_genuine.mp3?Expires=1587228202&OSSAccessKeyId=TMP.3KfDWMD2Es63VZdTpWymksHtsZNg8U4s4Zq2bbG9ym4fR43CpnpNwUe3bqZaXj9ueTFQUZU2ZLPM5D9dA8zZTqdNgUyrX6&Signature=WZpie2CP5fOHjIL6dwyV3Ar9lo8%3D");
                //                mDzhMediaPlayer.testFFmpeg()
                mDzhMediaPlayer.setPlayerPrepareListener(new PlayerPrepareListener() {
                    @Override
                    public void onPrepared() {
                        Log.i("MFFMPEG", "音频已经准备完成");
                        mDzhMediaPlayer.mStart();
                    }
                });
                mDzhMediaPlayer.mprepare();

                break;
            case R.id.btnPause:
                mDzhMediaPlayer.pause();
                break;
            case R.id.btnResume:
                mDzhMediaPlayer.resume();
                break;
            case R.id.btnStop:
                mDzhMediaPlayer.stop();
                break;
            case R.id.btnLeft:
                mDzhMediaPlayer.volumeLeft();
                break;
            case R.id.btnRight:
                mDzhMediaPlayer.volumeRight();
                break;
            case R.id.btnLeftRight:
                mDzhMediaPlayer.volumeLeftRight();
                break;
            case R.id.btnPitch1:
                mDzhMediaPlayer.setPitch(1);
                break;
            case R.id.btnPitch15:
                mDzhMediaPlayer.setPitch(1.5);
                break;
            case R.id.btnTempo1:
                mDzhMediaPlayer.setTempo(1);
                break;
            case R.id.btnTempo2:
                mDzhMediaPlayer.setTempo(2);
                break;
            case R.id.btnStartRecord:
                File outFile = new File(Environment.getExternalStorageDirectory(), "pcmToAac.aac");
                if (outFile.exists()) {
                    outFile.delete();
                }
                mDzhMediaPlayer.startRecord(outFile);
                break;
            case R.id.btnStopRecord:
                mDzhMediaPlayer.stopRecord();
                break;
            case R.id.btnPuaseRecord:
                mDzhMediaPlayer.paauseRecord();
                break;
            case R.id.btnContinueRecord:
                mDzhMediaPlayer.continueRecord();
                break;
            case R.id.btnCutAudioPlay:
                File cut = new File(Environment.getExternalStorageDirectory(), "bb.mp3");
                mDzhMediaPlayer.setSource(cut.getAbsolutePath());
                mDzhMediaPlayer.setPlayerPrepareListener(new PlayerPrepareListener() {
                    @Override
                    public void onPrepared() {
                        mDzhMediaPlayer.mCutAudioPlay(20, 40, true);
                    }
                });
                mDzhMediaPlayer.mprepare();

                break;

            case R.id.btnOpenGLES:

                startActivity(new Intent(this, TestOpenGlesActivity.class));
                break;
        }
    }
}

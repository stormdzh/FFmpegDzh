package com.stormdzh.ffmpegdzh;

import android.Manifest;
import android.app.Activity;
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
import com.stormdzh.libaudio.util.TestJni;

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

    private TestJni mTestJni;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        requestPermissions();

        tvTest = findViewById(R.id.tvTest);
        tvVersion = findViewById(R.id.tvVersion);
        tvProgress = findViewById(R.id.tvProgress);
        mSeekBar = findViewById(R.id.mSeekBar);
        mVolume = findViewById(R.id.mVolume);

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


        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean isfromUser) {

                if (isfromUser) {

                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                int progress = seekBar.getProgress();
                if (mTestJni != null) {
                    int seekto = (int) (mduration * ((double) progress / 100));
                    Log.i(TAG, "seek 时间：" + seekto);
                    mTestJni.seekto(seekto);
                }
            }
        });

        mVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean b) {
                Log.i(TAG, "seek 音量：" + progress);
                mTestJni.setVolume(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });


        mTestJni = new TestJni();
        tvTest.setText(mTestJni.stringFromJni());
        tvVersion.setText("获取到ffmpeg版本号:" + mTestJni.testFFmpeg());

        mTestJni.setOnPlayEventListener(new OnPlayEventListener() {

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
                mTestJni.normalThread();
                break;
            case R.id.btnStopNormalThread:
                mTestJni.stopNormalThread();
                break;
            case R.id.btnCoustom:
                mTestJni.mutexThread();
                mTestJni.callbackFromC();
                mTestJni.setOnErrerListener(new TestJni.OnErrerListener() {
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
                mTestJni.playPcm(pcmFile.getAbsolutePath());
                break;
            case R.id.btnStart:
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "3_test.wav");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "mp3test.mp3");
                File mp3File = new File(Environment.getExternalStorageDirectory(), "bb.mp3");
//                File mp3File = new File(Environment.getExternalStorageDirectory(), "aa.mp3");
                if (!mp3File.exists()) {
                    Log.i(TAG, "文件不存在");
                    return;
                }
                Log.i(TAG, "btnStart");
                mTestJni.setSource(mp3File.getAbsolutePath());
//                mTestJni.setSource("https://wp.zp68.com/sub/filestores/2016/07/20/2ad2953e033490cced7ab572564e84b5.mp3");
//                 mTestJni.setSource("http://stormful.oss-cn-shanghai.aliyuncs.com/0era/english/exam/10001_genuine.mp3?Expires=1587228202&OSSAccessKeyId=TMP.3KfDWMD2Es63VZdTpWymksHtsZNg8U4s4Zq2bbG9ym4fR43CpnpNwUe3bqZaXj9ueTFQUZU2ZLPM5D9dA8zZTqdNgUyrX6&Signature=WZpie2CP5fOHjIL6dwyV3Ar9lo8%3D");
                //                mTestJni.testFFmpeg()
                mTestJni.setPlayerPrepareListener(new PlayerPrepareListener() {
                    @Override
                    public void onPrepared() {
                        Log.i("MFFMPEG", "音频已经准备完成");
                        mTestJni.mStart();
                    }
                });
                mTestJni.mprepare();

                break;
            case R.id.btnPause:
                mTestJni.pause();
                break;
            case R.id.btnResume:
                mTestJni.resume();
                break;
            case R.id.btnStop:
                mTestJni.stop();
                break;
            case R.id.btnLeft:
                mTestJni.volumeLeft();
                break;
            case R.id.btnRight:
                mTestJni.volumeRight();
                break;
            case R.id.btnLeftRight:
                mTestJni.volumeLeftRight();
                break;
            case R.id.btnPitch1:
                mTestJni.setPitch(1);
                break;
            case R.id.btnPitch15:
                mTestJni.setPitch(1.5);
                break;
            case R.id.btnTempo1:
                mTestJni.setTempo(1);
                break;
            case R.id.btnTempo2:
                mTestJni.setTempo(2);
                break;
            case R.id.btnStartRecord:
                File outFile = new File(Environment.getExternalStorageDirectory(), "pcmToAac.aac");
                mTestJni.startRecord(outFile);
                break;
        }
    }
}

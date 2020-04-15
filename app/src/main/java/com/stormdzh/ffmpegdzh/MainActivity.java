package com.stormdzh.ffmpegdzh;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.stormdzh.libaudio.util.TestJni;

public class MainActivity extends Activity {

    private static final String TAG = "MFFMPEG";

    private TextView tvTest;
    private TextView tvVersion;

    private TestJni mTestJni;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvTest = findViewById(R.id.tvTest);
        tvVersion = findViewById(R.id.tvVersion);

        mTestJni = new TestJni();


        Log.i(TAG, "getStringFromJNI:" + mTestJni.getStringFromJNI());
        tvTest.setText(mTestJni.getStringFromJNI());
        Log.i(TAG, "获取到ffmpeg版本号:" + mTestJni.getTestFFmpeg());
        tvVersion.setText("获取到ffmpeg版本号:" + mTestJni.getTestFFmpeg());
    }

}

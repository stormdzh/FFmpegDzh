package com.stormdzh.ffmpegdzh;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.stormdzh.libaudio.util.TestJni;

public class MainActivity extends Activity {

    private static final String TAG = "MFFMPEG";
    private TestJni mTestJni;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTestJni = new TestJni();


        Log.i(TAG, "getStringFromJNI:" + mTestJni.getStringFromJNI());
    }
}

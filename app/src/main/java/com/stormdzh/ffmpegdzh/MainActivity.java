package com.stormdzh.ffmpegdzh;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.stormdzh.libaudio.util.TestJni;

public class MainActivity extends Activity implements View.OnClickListener {

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

        findViewById(R.id.btnNormalThread).setOnClickListener(this);
        findViewById(R.id.btnStopNormalThread).setOnClickListener(this);
        findViewById(R.id.btnCoustom).setOnClickListener(this);


        mTestJni = new TestJni();


        Log.i(TAG, "getStringFromJNI:" + mTestJni.getStringFromJNI());
        tvTest.setText(mTestJni.getStringFromJNI());
        Log.i(TAG, "获取到ffmpeg版本号:" + mTestJni.getTestFFmpeg());
        tvVersion.setText("获取到ffmpeg版本号:" + mTestJni.getTestFFmpeg());
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.btnNormalThread:
                mTestJni.normalThread();
                break;
            case R.id.btnStopNormalThread:
                mTestJni.stopNormalThread();
                break;
            case R.id.btnCoustom:
                mTestJni.mutexThread();
                mTestJni.callbackFromC();
                mTestJni.setOnErrerListener(new TestJni.OnErrerListener(){
                    @Override
                    public void onError(int code, String msg) {
                        Log.i("MFFMPEG","code:"+code+"  msg:"+msg);
                    }
                });
                break;
        }
    }
}

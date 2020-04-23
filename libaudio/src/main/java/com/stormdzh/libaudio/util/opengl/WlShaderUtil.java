package com.stormdzh.libaudio.util.opengl;

import android.content.Context;
import android.opengl.GLES20;
import android.util.Log;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * @Description: 描述
 * @Author: dzh
 * @CreateDate: 2020-04-22 10:48
 */
public class WlShaderUtil {

    public static String readRawText(Context context, int rawId) {
        InputStream inputStream = context.getResources().openRawResource(rawId);
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

        StringBuffer sb = new StringBuffer();
        String line;
        try {
            while ((line = reader.readLine()) != null) {

                sb.append(line).append("\n");
            }
            reader.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return sb.toString();
    }

    public static int loadShader(int shaderType, String source) {

        int shader = GLES20.glCreateShader(shaderType);
        if (shader != 0) {
            GLES20.glShaderSource(shader, source);
            GLES20.glCompileShader(shader);
            int[] compile = new int[1];
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compile, 0);
            if (compile[0] != GLES20.GL_TRUE) {
                Log.i("WlShaderUtil", " compile 失败");
                GLES20.glDeleteShader(shader);
                shader = 0;
            }

        }
        return shader;

    }

    public static int creteProgram(String vertexSource, String fragmentSource) {

        int vettexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource);
        if (vettexShader == 0) {

            return 0;
        }

        int fragmentShaper = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource);

        if (fragmentShaper == 0) {

            return 0;
        }

        int program = GLES20.glCreateProgram();

        if (program != 0) {
            GLES20.glAttachShader(program, vettexShader);
            GLES20.glAttachShader(program, fragmentShaper);
            GLES20.glLinkProgram(program);
            int[] linsStatus = new int[1];
            GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linsStatus, 0);
            if (linsStatus[0] != GLES20.GL_TRUE) {
                Log.i("WlShaderUtil", " linsStatus 失败");
                GLES20.glDeleteProgram(program);
                program = 0;

            }
        }

        return program;
    }
}

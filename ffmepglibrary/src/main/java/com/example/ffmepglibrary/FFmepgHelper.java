package com.example.ffmepglibrary;

public class FFmepgHelper {

    static {
        System.loadLibrary("native-lib");
    }

    public static native String getFFmpegVersion();

    public static native String getFFmpegConfiguration();
}

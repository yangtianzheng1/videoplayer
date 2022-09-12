package com.example.ffmepglibrary;

import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;

public class VideoPlayer {

    private String mLocalPath;

    private HandlerThread handlerThread;
    private Handler handler;


    public void playVideo(String localPath, Surface surface, int width, int height){
        if (handlerThread == null){
            handlerThread = new HandlerThread("VideoPlayer");
            handlerThread.start();
            handler = new Handler(handlerThread.getLooper());
        }
        handler.post(new Runnable() {
            @Override
            public void run() {
                int result = init(localPath, surface, width, height);
                if (result >= 0){
                    start();
                    release();
                }
            }
        });
    }


    public void stopVideo(){
        if (handlerThread != null){
            handlerThread.quitSafely();
        }
    }

    private native int init(String mLocalPath , Surface surface, int width, int height);
    private native void start();
    private native void release();
}

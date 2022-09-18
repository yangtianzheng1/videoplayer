package com.example.ffmepglibrary;

import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;

public class VideoPlayer {

    static {
        System.loadLibrary("native-lib");
    }

    public static final int MSG_DECODER_INIT_ERROR      = 0;
    public static final int MSG_DECODER_READY           = 1;
    public static final int MSG_DECODER_DONE            = 2;
    public static final int MSG_REQUEST_RENDER          = 3;
    public static final int MSG_DECODING_TIME           = 4;

    private EventCallback mEventCallback = null;

    private long mNativeEngineCtr = -2;

    public VideoPlayer() {
        mNativeEngineCtr = nativeCreateMediaPlayEngine();
    }

    public void destroy() {
        nativeDestroy(mNativeEngineCtr);
    }

    public void init() {
        nativeInit(mNativeEngineCtr);
    }

    public void play(String url, Surface surface, int width, int height) {
        nativePlay(mNativeEngineCtr, url, surface, width, height);
    }

    public void pause() {
        nativePause(mNativeEngineCtr);
    }

    public void stop() {
        nativeStop(mNativeEngineCtr);
    }

    public void seekToPosition(float position) {
        nativeSeekToPosition(mNativeEngineCtr, position);
    }

    public long getMediaParams(int paramType) {
        return nativeGetMediaParams(mNativeEngineCtr, paramType);
    }

    public interface EventCallback {
        void onPlayerEvent(int msgType, float msgValue);
    }

    private void playerEventCallback(int msgType, float msgValue) {
        if (mEventCallback != null)
            mEventCallback.onPlayerEvent(msgType, msgValue);
    }

    public void addEventCallback(EventCallback callback) {
        mEventCallback = callback;
    }

    public native  long nativeCreateMediaPlayEngine();

    public native  void nativeDestroy(long nativeEngineCtr);

    public native  void nativeInit(long nativeEngineCtr);

    public native  void nativePlay(long nativeEngineCtr,String url, Surface surface, int width, int height);

    public native  void nativePause(long nativeEngineCtr);

    public native  void nativeStop(long nativeEngineCtr);

    public native  void nativeSeekToPosition(long nativeEngineCtr, float position);

    public native  long nativeGetMediaParams(long nativeEngineCtr, int paramType);
}

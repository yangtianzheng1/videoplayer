package com.example.videoplayer;

import static android.Manifest.permission.READ_EXTERNAL_STORAGE;
import static android.widget.Toast.LENGTH_SHORT;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.example.ffmepglibrary.FFmepgHelper;
import com.example.ffmepglibrary.VideoPlayer;

import java.io.File;

import util.GetPathFromUri4kitkat;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private TextView tvContent;
    private VideoPlayer videoPlayer;
    private String mp4LocalPath;
    private Surface surface;
    private int surfaceWidth;
    private int surfaceHeight;

    private TextView textView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvContent = findViewById(R.id.tv_content);
        tvContent.setText(FFmepgHelper.getFFmpegVersion());
        surfaceView = findViewById(R.id.surfaceview);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                Log.d("YTZ", Thread.currentThread().getName());
                surface = holder.getSurface();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                surfaceWidth = width;
                surfaceHeight = height;
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                surface = null;
            }
        });


        findViewById(R.id.tv_video).setOnClickListener(
                v -> {
                    Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                    intent.setType("video/mp4"); //选择视频 （mp4 3gp 是android支持的视频格式）
                    intent.addCategory(Intent.CATEGORY_OPENABLE);
                    startActivityForResult(intent, 1);
                }
        );

        findViewById(R.id.tv_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playVideo();
            }
        });

        requestMyPermissions();
    }

    private void requestMyPermissions() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
        } else {
            Log.d("YTZ", "requestMyPermissions: 有写SD权限");
        }
        if (ContextCompat.checkSelfPermission(this,
                READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{READ_EXTERNAL_STORAGE}, 100);
        } else {
            Log.d("YTZ", "requestMyPermissions: 有读SD权限");
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK) {
            String path = getRealFilePath(data.getData());
            if (path != null) {
                mp4LocalPath = path;
            }
        }
    }

    public void playVideo(){
        if (TextUtils.isEmpty(mp4LocalPath)){
            Toast.makeText(this, "视频路径为空", LENGTH_SHORT).show();
            return;
        }
        if (surface == null){
            Toast.makeText(this, "surface 为空", LENGTH_SHORT).show();
            return;
        }
        if (surfaceWidth == 0 || surfaceHeight == 0){
            Toast.makeText(this, "宽高不正确", LENGTH_SHORT).show();
            return;
        }
        File file = new File(mp4LocalPath);
        if (file.exists() && file.length() > 0){
            if (videoPlayer == null){
                videoPlayer = new VideoPlayer();
            }
            videoPlayer.playVideo(mp4LocalPath, surface, surfaceWidth, surfaceHeight);
        }
    }

    public String getRealFilePath(final Uri uri) {
        if (null == uri) return null;
        final String scheme = uri.getScheme();
        String data = null;
        if (scheme == null) {
            Log.e("wuwang", "scheme is null");
            data = uri.getPath();
        } else if (ContentResolver.SCHEME_FILE.equals(scheme)) {
            data = uri.getPath();
            Log.e("wuwang", "SCHEME_FILE");
        } else if (ContentResolver.SCHEME_CONTENT.equals(scheme)) {
            data = GetPathFromUri4kitkat.getPath(getApplicationContext(), uri);
        }
        return data;
    }
}
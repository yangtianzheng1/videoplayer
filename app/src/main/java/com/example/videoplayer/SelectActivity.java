package com.example.videoplayer;

import static android.Manifest.permission.READ_EXTERNAL_STORAGE;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import util.GetPathFromUri4kitkat;

public class SelectActivity extends AppCompatActivity {

    public static final String videoPath = "path";
    public static final String videoPath2 = "path2";

    private String mp4LocalPath;

    private String mp4LocalPath2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select);
        Button openFile = findViewById(R.id.btn_file);

        openFile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("video/mp4"); //选择视频 （mp4 3gp 是android支持的视频格式）
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                startActivityForResult(intent, 1);
            }
        });

        findViewById(R.id.btn_file2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.setType("video/mp4"); //选择视频 （mp4 3gp 是android支持的视频格式）
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                startActivityForResult(intent, 2);
            }
        });

        findViewById(R.id.btn_codec1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!mp4LocalPath.isEmpty()){
                    MediaCodec1Activity.Companion.startActivity(SelectActivity.this, mp4LocalPath);
                }
            }
        });

        findViewById(R.id.btn_codec).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!mp4LocalPath.isEmpty()){
                    MediaCodecActivity.Companion.startActivity(SelectActivity.this, mp4LocalPath);
                }
            }
        });

        findViewById(R.id.btn_gl1).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(SelectActivity.this, SimpleRenderActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.btn_gl2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(!mp4LocalPath.isEmpty()){
                    Intent intent = new Intent(SelectActivity.this, OpenGLPlayerActivity.class);
                    intent.putExtra(videoPath, mp4LocalPath);
                    startActivity(intent);
                }
            }
        });

        findViewById(R.id.btn_gl3).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mp4LocalPath.isEmpty() && !mp4LocalPath2.isEmpty()){
                    Intent intent = new Intent(SelectActivity.this, MultiOpenGLPlayerActivity.class);
                    intent.putExtra(videoPath, mp4LocalPath);
                    intent.putExtra(videoPath2, mp4LocalPath2);
                    startActivity(intent);
                }
            }
        });

        findViewById(R.id.btn_egl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mp4LocalPath.isEmpty() && !mp4LocalPath2.isEmpty()){
                    Intent intent = new Intent(SelectActivity.this, EGLPlayerActivity.class);
                    intent.putExtra(videoPath, mp4LocalPath);
                    intent.putExtra(videoPath2, mp4LocalPath2);
                    startActivity(intent);
                }
            }
        });

        findViewById(R.id.btn_soul).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mp4LocalPath.isEmpty()){
                    Intent intent = new Intent(SelectActivity.this, SoulPlayerActivity.class);
                    intent.putExtra(videoPath, mp4LocalPath);
                    startActivity(intent);
                }
            }
        });

        findViewById(R.id.btn_create).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mp4LocalPath.isEmpty()){
                    Intent intent = new Intent(SelectActivity.this, SynthesizerActivity.class);
                    intent.putExtra(videoPath, mp4LocalPath);
                    startActivity(intent);
                }
            }
        });

        findViewById(R.id.btn_ffmpeg_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(SelectActivity.this, FFmpegActivity.class);
                intent.putExtra(videoPath, mp4LocalPath);
                startActivity(intent);
            }
        });

        findViewById(R.id.btn_ffmpeg_gl_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(SelectActivity.this, FFmpegGLPlayerActivity.class);
                intent.putExtra(videoPath, mp4LocalPath);
                startActivity(intent);
            }
        });

        requestMyPermissions();
    }

    private void requestMyPermissions() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(SelectActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 100);
        } else {
            Log.d("YTZ", "requestMyPermissions: 有写SD权限");
        }
        if (ContextCompat.checkSelfPermission(this,
                READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            //没有授权，编写申请权限代码
            ActivityCompat.requestPermissions(SelectActivity.this, new String[]{READ_EXTERNAL_STORAGE}, 100);
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
                Log.e("mp4Path", path);
                mp4LocalPath = path;
            }
        }

        if (requestCode == 2 && resultCode == RESULT_OK) {
            String path = getRealFilePath(data.getData());
            if (path != null) {
                mp4LocalPath2 = path;
            }
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
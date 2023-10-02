package com.example.videoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.Toast
import java.io.File

class FFmpegActivity : AppCompatActivity() {

    private var player: Long? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffmpeg)

        val path = intent.getStringExtra(SelectActivity.videoPath)

        if (!path.isNullOrEmpty()){
            initSfv(path)
        }
    }

    private fun initSfv(path: String) {
        if (File(path).exists()) {
            findViewById<SurfaceView>(R.id.sfv).holder.addCallback(object : SurfaceHolder.Callback {
                override fun surfaceChanged(holder: SurfaceHolder, format: Int,
                                            width: Int, height: Int) {}
                override fun surfaceDestroyed(holder: SurfaceHolder) {}

                override fun surfaceCreated(holder: SurfaceHolder) {
                    if (player == null) {
                        player = createPlayer(path, holder.surface)
                        play(player!!)
                    }
                }
            })
        } else {
            Toast.makeText(this, "视频文件不存在，请在手机根目录下放置 mvtest.mp4", Toast.LENGTH_SHORT).show()
        }
    }

    private external fun createPlayer(path: String, surface: Surface): Long

    private external fun play(player: Long)

    private external fun pause(player: Long)

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
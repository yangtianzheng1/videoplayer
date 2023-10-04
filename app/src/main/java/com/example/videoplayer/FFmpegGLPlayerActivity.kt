package com.example.videoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class FFmpegGLPlayerActivity : AppCompatActivity() {

    var path: String? =null

    private var player: Long? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffmpeg_glplayer)
        path = intent.getStringExtra(SelectActivity.videoPath);

        if (!path.isNullOrEmpty()){
            findViewById<SurfaceView>(R.id.sfv).holder.addCallback(object : SurfaceHolder.Callback {
                override fun surfaceChanged(holder: SurfaceHolder, format: Int,
                                            width: Int, height: Int) {}
                override fun surfaceDestroyed(holder: SurfaceHolder) {
                    stop(player!!)
                }

                override fun surfaceCreated(holder: SurfaceHolder) {
                    if (player == null) {
                        player = createGLPlayer(path!!, holder.surface)
                        playOrPause(player!!)
                    }
                }
            })
        }
    }

    private external fun createGLPlayer(path: String, surface: Surface): Long
    private external fun playOrPause(player: Long)
    private external fun stop(player: Long)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}
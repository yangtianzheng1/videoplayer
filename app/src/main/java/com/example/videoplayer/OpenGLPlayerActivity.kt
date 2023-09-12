package com.example.videoplayer

import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.VideoDecoder
import com.example.videoplayer.opengl.drawer.IDrawer
import com.example.videoplayer.opengl.drawer.VideoDrawer
import com.example.videoplayer.opengl.render.SimpleRender
import java.util.concurrent.Executors

class OpenGLPlayerActivity : AppCompatActivity() {

    private var videoPath: String? = null
    private lateinit var surface: GLSurfaceView
    private lateinit var drawer: IDrawer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_open_glplayer)

        surface = findViewById(R.id.gl_surface)

        videoPath = intent.getStringExtra(SelectActivity.videoPath)

        initRender()
    }

    private fun initRender(){
        drawer  = VideoDrawer()
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(Surface( it))
        }
        surface.setEGLContextClientVersion(2)
        val render = SimpleRender()
        render.addDrawer(drawer)
        surface.setRenderer(render)
    }

    private fun initPlayer(sf: Surface) {
        val threadPool = Executors.newFixedThreadPool(10)

        val videoDecoder = VideoDecoder(videoPath!!, null, sf)
        threadPool.execute(videoDecoder)

        val audioDecoder = AudioDecoder(videoPath!!)
        threadPool.execute(audioDecoder)

        videoDecoder.goOn()
        audioDecoder.goOn()
    }
}
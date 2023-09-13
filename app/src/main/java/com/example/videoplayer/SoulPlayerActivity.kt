package com.example.videoplayer

import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.VideoDecoder
import com.example.videoplayer.opengl.drawer.IDrawer
import com.example.videoplayer.opengl.drawer.SoulVideoDrawer
import com.example.videoplayer.opengl.render.SimpleRender
import java.util.concurrent.Executors

class SoulPlayerActivity : AppCompatActivity() {

    private var videoPath: String? = null
    private lateinit var surface: GLSurfaceView
    private lateinit var drawer: IDrawer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_soul_player)
        videoPath = intent.getStringExtra(SelectActivity.videoPath)

        surface = findViewById(R.id.gl_soul)

        initRender()
    }
    private fun initRender() {
        // 使用“灵魂出窍”渲染器
        drawer = SoulVideoDrawer()
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(Surface(it))
        }
        surface.setEGLContextClientVersion(2)
        val render = SimpleRender()
        render.addDrawer(drawer)
        surface.setRenderer(render)
    }

    private fun initPlayer(sf: Surface) {
        val threadPool = Executors.newFixedThreadPool(4)

        val videoDecoder = VideoDecoder(videoPath!!, null, sf)
        threadPool.execute(videoDecoder)

        val audioDecoder = AudioDecoder(videoPath!!)
        threadPool.execute(audioDecoder)

        videoDecoder.goOn()
        audioDecoder.goOn()
    }
}
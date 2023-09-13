package com.example.videoplayer

import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.view.Surface
import android.view.SurfaceView
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.VideoDecoder
import com.example.videoplayer.opengl.drawer.VideoDrawer
import com.example.videoplayer.opengl.render.CustomerGLRenderer
import com.example.videoplayer.opengl.render.SimpleRender
import java.util.concurrent.Executors

class EGLPlayerActivity : AppCompatActivity() {

    private var videoPath1: String? = null
    private var videoPath2: String? = null
    private lateinit var surface: SurfaceView

    private val render = CustomerGLRenderer()

    private val threadPool = Executors.newFixedThreadPool(4)


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_eglplayer)

        surface = findViewById(R.id.surfaceview)
        videoPath1 = intent.getStringExtra(SelectActivity.videoPath)
        videoPath2 = intent.getStringExtra(SelectActivity.videoPath2)

        initFirstVideo()
        initSecondVideo()
        setRenderSurface()
    }


    private fun initFirstVideo() {
        val drawer = VideoDrawer()
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(videoPath1!!, Surface(it), true)
        }
        render.addDrawer(drawer)
    }

    private fun initSecondVideo() {
        val drawer = VideoDrawer()
        drawer.setAlpha(0.5f)
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initPlayer(videoPath2!!, Surface(it), false)
        }
        render.addDrawer(drawer)

        Handler().postDelayed({
            drawer.scale(0.8f, 0.8f)
        }, 1000)
    }

    private fun initPlayer(path: String, sf: Surface, withSound: Boolean) {
        val videoDecoder = VideoDecoder(path, null, sf)
        threadPool.execute(videoDecoder)
        videoDecoder.goOn()

        if (withSound) {
            val audioDecoder = AudioDecoder(path)
            threadPool.execute(audioDecoder)
            audioDecoder.goOn()
        }
    }

    private fun setRenderSurface() {
        render.setSurface(surface)
    }

}
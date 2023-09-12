package com.example.videoplayer

import android.graphics.BitmapFactory
import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.example.videoplayer.opengl.drawer.BitmapDrawer
import com.example.videoplayer.opengl.drawer.IDrawer
import com.example.videoplayer.opengl.drawer.TriangleDrawer
import com.example.videoplayer.opengl.render.SimpleRender

class SimpleRenderActivity : AppCompatActivity() {

    private lateinit var drawer: IDrawer
    private var surface: GLSurfaceView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_simple_render)

        surface = findViewById(R.id.gl_surface)

       val bitmap =  BitmapFactory.decodeResource(this.resources, R.drawable.pic_1)

        drawer = BitmapDrawer(bitmap)

        initRender()
    }

    private fun initRender(){
        surface?.setEGLContextClientVersion(2)
        val render = SimpleRender()
        render.addDrawer(drawer)
        surface?.setRenderer(render)
    }

    override fun onDestroy() {
        drawer.release()
        super.onDestroy()
    }
}
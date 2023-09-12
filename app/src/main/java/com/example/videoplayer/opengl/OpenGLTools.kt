package com.example.videoplayer.opengl

import android.opengl.GLES20

object OpenGLTools {
    fun createTextureIds(count: Int):IntArray{
        val texture = IntArray(count)
        GLES20.glGenBuffers(count, texture, 0)
        return texture
    }
}
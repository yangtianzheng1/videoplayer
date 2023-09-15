package com.example.videoplayer.encoder

interface IEncodeStateListener {
    fun encodeStart(encoder: BaseEncoder)
    fun encodeProgress(encoder: BaseEncoder)
    fun encoderFinish(encoder: BaseEncoder)
}
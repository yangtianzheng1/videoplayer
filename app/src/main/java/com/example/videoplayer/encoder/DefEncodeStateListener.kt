package com.example.videoplayer.encoder

interface DefEncodeStateListener: IEncodeStateListener {

    override fun encodeStart(encoder: BaseEncoder) {

    }

    override fun encodeProgress(encoder: BaseEncoder) {

    }

    override fun encoderFinish(encoder: BaseEncoder) {

    }
}
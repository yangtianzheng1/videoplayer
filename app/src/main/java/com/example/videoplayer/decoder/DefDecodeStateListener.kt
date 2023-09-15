package com.example.videoplayer.decoder

import com.example.videoplayer.Frame

interface DefDecodeStateListener: IDecoderStateListener {
    override fun decoderPrepare(decodeJob: BaseDecoder?) {}
    override fun decoderReady(decodeJob: BaseDecoder?) {}
    override fun decoderRunning(decodeJob: BaseDecoder?) {}
    override fun decoderPause(decodeJob: BaseDecoder?) {}
    override fun decodeOneFrame(decodeJob: BaseDecoder?, frame: Frame) {}
    override fun decoderFinish(decodeJob: BaseDecoder?) {}
    override fun decoderDestroy(decodeJob: BaseDecoder?) {}
    override fun decoderError(decodeJob: BaseDecoder?, msg: String) {}
}
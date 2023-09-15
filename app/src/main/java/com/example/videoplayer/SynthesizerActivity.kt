package com.example.videoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import android.widget.Button
import com.example.videoplayer.SelectActivity.videoPath
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.BaseDecoder
import com.example.videoplayer.decoder.DefDecodeStateListener
import com.example.videoplayer.decoder.IDecoder
import com.example.videoplayer.decoder.VideoDecoder
import com.example.videoplayer.encoder.AudioEncoder
import com.example.videoplayer.encoder.BaseEncoder
import com.example.videoplayer.encoder.DefEncodeStateListener
import com.example.videoplayer.encoder.VideoEncoder
import com.example.videoplayer.muxer.MMuxer
import com.example.videoplayer.opengl.drawer.SoulVideoDrawer
import com.example.videoplayer.opengl.drawer.VideoDrawer
import com.example.videoplayer.opengl.render.CustomerGLRenderer
import java.util.concurrent.Executors

class SynthesizerActivity : AppCompatActivity(), MMuxer.IMuxerStateListener {

    private var path: String? = null

    private val threadPool = Executors.newFixedThreadPool(6)

    private var renderer = CustomerGLRenderer()

    private var audioDecoder: IDecoder? = null
    private var videoDecoder: IDecoder? = null

    private lateinit var videoEncoder: VideoEncoder
    private lateinit var audioEncoder: AudioEncoder

    private var muxer = MMuxer()

    private var button :Button? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_synthesizer)

        path = intent.getStringExtra(videoPath)

        muxer.setStateListener(this)
        button = findViewById<Button>(R.id.btn_start)
        button?.setOnClickListener {
            button?.text = "正在编码"
            button?.isEnabled = false
            initVideo()
            initAudio()
            initAudioEncoder()
            initVideoEncoder()
        }
    }

    private fun initVideoEncoder() {
        // 视频编码器
        videoEncoder = VideoEncoder(muxer, 1920, 1080)

        renderer.setRenderMode(CustomerGLRenderer.RenderMode.RENDER_WHEN_DIRTY)
        renderer.setSurface(videoEncoder.getEncodeSurface()!!, 1920, 1080)

        videoEncoder.setStateListener(object : DefEncodeStateListener {
            override fun encoderFinish(encoder: BaseEncoder) {
                renderer.stop()
            }
        })
        threadPool.execute(videoEncoder)
    }

    private fun initAudioEncoder() {
        // 音频编码器
        audioEncoder = AudioEncoder(muxer)
        // 启动编码线程
        threadPool.execute(audioEncoder)
    }

    private fun initVideo() {
        val drawer = VideoDrawer()
//        val drawer = SoulVideoDrawer()
        drawer.setVideoSize(1920, 1080)
        drawer.getSurfaceTexture {
            initVideoDecoder(path!!, Surface(it))
        }
        renderer.addDrawer(drawer)
    }

    private fun initVideoDecoder(path: String, sf: Surface) {
        videoDecoder?.stop()
        videoDecoder = VideoDecoder(path, null, sf).withoutSync()
        videoDecoder!!.setStateListener(object : DefDecodeStateListener {
            override fun decodeOneFrame(decodeJob: BaseDecoder?, frame: Frame) {
                renderer.notifySwap(frame.bufferInfo.presentationTimeUs)
                videoEncoder.encodeOneFrame(frame)
            }

            override fun decoderFinish(decodeJob: BaseDecoder?) {
                videoEncoder.endOfStream()
            }
        })
        videoDecoder!!.goOn()

        //启动解码线程
        threadPool.execute(videoDecoder!!)
    }

    private fun initAudio() {
        audioDecoder?.stop()
        audioDecoder = AudioDecoder(path!!).withoutSync()
        audioDecoder!!.setStateListener(object : DefDecodeStateListener {

            override fun decodeOneFrame(decodeJob: BaseDecoder?, frame: Frame) {
                audioEncoder.encodeOneFrame(frame)
            }

            override fun decoderFinish(decodeJob: BaseDecoder?) {
                audioEncoder.endOfStream()
            }
        })
        audioDecoder!!.goOn()

        //启动解码线程
        threadPool.execute(audioDecoder!!)
    }

    override fun onMuxerFinish() {

        runOnUiThread {
            button?.isEnabled = true
            button?.text = "编码完成"
        }

        audioDecoder?.stop()
        audioDecoder = null

        videoDecoder?.stop()
        videoDecoder = null
    }
}
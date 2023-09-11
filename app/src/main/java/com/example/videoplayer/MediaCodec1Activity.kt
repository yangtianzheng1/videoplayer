package com.example.videoplayer

import android.app.Activity
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceView
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.VideoDecoder
import java.util.concurrent.Executors

class MediaCodec1Activity : AppCompatActivity() {


    companion object{

        fun startActivity(context: Activity, path: String){
            val intent = Intent(
                context,
                MediaCodec1Activity::class.java
            )
            intent.putExtra(SelectActivity.videoPath, path)
            context.startActivity(intent)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mediacodec)
        val surView = findViewById<SurfaceView>(R.id.sfv_codec)

        initPlayer(intent.getStringExtra(SelectActivity.videoPath)!!, surView)
    }

    private fun initPlayer(path :String, surfaceView: SurfaceView) {

        //创建线程池
        val threadPool = Executors.newFixedThreadPool(2)

        //创建视频解码器
        val videoDecoder = VideoDecoder(path, surfaceView, null)
        threadPool.execute(videoDecoder)

        //创建音频解码器
        val audioDecoder = AudioDecoder(path)
        threadPool.execute(audioDecoder)

        //开启播放
        videoDecoder.goOn()
        audioDecoder.goOn()
    }
}
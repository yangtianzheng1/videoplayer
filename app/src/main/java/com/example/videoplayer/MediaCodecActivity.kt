package com.example.videoplayer

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.view.SurfaceView
import androidx.appcompat.app.AppCompatActivity
import com.example.videoplayer.decoder.AudioDecoder
import com.example.videoplayer.decoder.VideoDecoder
import com.example.videoplayer.muxer.MP4Repack
import java.util.concurrent.Executors

class MediaCodecActivity :AppCompatActivity(){

    companion object{

        fun startActivity(context: Activity, path: String){
            val intent = Intent(
                context,
                MediaCodecActivity::class.java
            )
            intent.putExtra(SelectActivity.videoPath, path)
            context.startActivity(intent)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mediacodec)
        repack(intent.getStringExtra(SelectActivity.videoPath)!!)
    }

    private fun repack(path :String) {

        val repack = MP4Repack(path)
        repack.start()
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
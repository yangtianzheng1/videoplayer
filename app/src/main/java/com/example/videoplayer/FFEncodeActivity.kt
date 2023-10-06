package com.example.videoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.widget.Button
import android.widget.Toast
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date

class FFEncodeActivity : AppCompatActivity() {

    private var ffEncoder: Long = -1
    private var path: String? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffencode)

        path = intent.getStringExtra(SelectActivity.videoPath)

        findViewById<Button>(R.id.btn_start_repack1).setOnClickListener {
            if (ffEncoder > 0){
                startEncoder(ffEncoder)
                Toast.makeText(this, "开始编码", Toast.LENGTH_SHORT).show()
            }
        }

        val fileName = "Video_Test" + SimpleDateFormat("yyyyMM_dd-HHmmss").format(Date()) + ".mp4"
        val filePath = Environment.getExternalStorageDirectory().absolutePath.toString() + "/"
        val mPath = filePath + fileName
        val file = File(mPath)
        if (!file.exists()){
            file.parentFile.mkdirs()
            file.createNewFile()
        }

        ffEncoder = initEncoder(path!!, mPath)
    }

    override fun onDestroy() {
        if (ffEncoder > 0) {
            releaseEncoder(ffEncoder)
        }
        super.onDestroy()
    }


    private external fun initEncoder(srcPath: String, destPath: String): Long

    private external fun startEncoder(encoder: Long)

    private external fun releaseEncoder(encoder: Long)

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
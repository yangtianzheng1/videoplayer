package com.example.videoplayer

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.widget.Button
import java.io.File
import java.text.SimpleDateFormat
import java.util.Date

class FFRepackActivity : AppCompatActivity() {

    private var path: String? = null

    private var repack: Long? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffrepack)

        path = intent.getStringExtra(SelectActivity.videoPath)

        findViewById<Button>(R.id.btn_start_repack).setOnClickListener {
            startRepack(repack!!)
        }

        val fileName = "Video_Test" + SimpleDateFormat("yyyyMM_dd-HHmmss").format(Date()) + ".mp4"
        val filePath = Environment.getExternalStorageDirectory().absolutePath.toString() + "/"
        val mPath = filePath + fileName
        val file = File(mPath)
        if (!file.exists()){
            file.parentFile.mkdirs()
            file.createNewFile()
        }

        repack = createRepack(path!!, mPath)

    }

    private external fun createRepack(srcPath :String, destPath :String) :Long

    private external fun startRepack(repack: Long)

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
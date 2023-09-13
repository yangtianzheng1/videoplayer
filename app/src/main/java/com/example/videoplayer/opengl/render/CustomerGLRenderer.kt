package com.example.videoplayer.opengl.render

import android.opengl.EGLExt.EGL_RECORDABLE_ANDROID
import android.opengl.GLES20
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import com.example.videoplayer.egl.EGLSurfaceHolder
import com.example.videoplayer.opengl.OpenGLTools
import com.example.videoplayer.opengl.drawer.IDrawer
import java.lang.ref.WeakReference

class CustomerGLRenderer:SurfaceHolder.Callback {

    //OpenGL渲染线程
    private val mThread = RenderThread()

    //页面上的SurfaceView弱引用
    private var mSurfaceView: WeakReference<SurfaceView>? = null

    //所有的绘制器
    private val mDrawers = mutableListOf<IDrawer>()

    init {
        mThread.start()
    }

    /**
     * 设置SurfaceView
     */
    fun setSurface(surface: SurfaceView) {
        mSurfaceView = WeakReference(surface)
        surface.holder.addCallback(this)

        surface.addOnAttachStateChangeListener(object : View.OnAttachStateChangeListener{
            override fun onViewDetachedFromWindow(v: View?) {
                mThread.onSurfaceStop()
            }

            override fun onViewAttachedToWindow(v: View?) {
            }
        })
    }

    /**
     * 添加绘制器
     */
    fun addDrawer(drawer: IDrawer) {
        mDrawers.add(drawer)
    }


    override fun surfaceCreated(holder: SurfaceHolder) {
        mThread.onSurfaceCreate()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        mThread.onSurfaceChange(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        mThread.onSurfaceDestroy()
    }

    inner class RenderThread: Thread(){
        // 渲染状态
        private var mState = RenderState.NO_SURFACE

        private var mEGLSurface: EGLSurfaceHolder? = null

        // 是否绑定了EGLSurface
        private var mHaveBindEGLContext = false

        //是否已经新建过EGL上下文，用于判断是否需要生产新的纹理ID
        private var mNeverCreateEglContext = true
        private var mWidth = 0
        private var mHeight = 0

        private val mWaitLock = Object()

        private fun holdOn() {
            synchronized(mWaitLock) {
                mWaitLock.wait()
            }
        }

        private fun notifyGo() {
            synchronized(mWaitLock) {
                mWaitLock.notify()
            }
        }

        fun onSurfaceCreate() {
            mState = RenderState.FRESH_SURFACE
            notifyGo()
        }

        fun onSurfaceChange(width: Int, height: Int) {
            mWidth = width
            mHeight = height
            mState = RenderState.SURFACE_CHANGE
            notifyGo()
        }

        fun onSurfaceDestroy() {
            mState = RenderState.SURFACE_DESTROY
            notifyGo()
        }

        fun onSurfaceStop() {
            mState = RenderState.STOP
            notifyGo()
        }

        override fun run(){
            initEGL()
            while (true){
                when(mState){
                    RenderState.FRESH_SURFACE ->{
                        //【2】使用surface初始化EGLSurface，并绑定上下文
                        createEGLSurfaceFirst()
                        holdOn()
                    }
                    RenderState.SURFACE_CHANGE ->{
                        createEGLSurfaceFirst()
                        //【3】初始化OpenGL世界坐标系宽高
                        GLES20.glViewport(0, 0, mWidth, mHeight)
                        configWordSize()
                        mState = RenderState.RENDERING
                    }
                    RenderState.RENDERING ->{
                        render()
                        sleep(20)
                    }
                    RenderState.SURFACE_DESTROY ->{
                        //【5】销毁EGLSurface，并解绑上下文
                        destroyEGLSurface()
                        mState = RenderState.NO_SURFACE
                    }
                    RenderState.STOP -> {
                        //【6】释放所有资源
                        releaseEGL()
                        return
                    }else ->{
                        holdOn()
                    }
                }
            }
        }

        private fun initEGL(){
            mEGLSurface = EGLSurfaceHolder()
            mEGLSurface?.init(null, EGL_RECORDABLE_ANDROID)
        }

        private fun createEGLSurfaceFirst() {
            if (!mHaveBindEGLContext) {
                mHaveBindEGLContext = true
                createEGLSurface()
                if (mNeverCreateEglContext) {
                    mNeverCreateEglContext = false
                    GLES20.glClearColor(0f, 0f, 0f, 0f)
                    //开启混合，即半透明
                    GLES20.glEnable(GLES20.GL_BLEND)
                    GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA)
                    generateTextureID()
                }
            }
        }

        private fun generateTextureID() {
            val textureIds = OpenGLTools.createTextureIds(mDrawers.size)
            for ((idx, drawer) in mDrawers.withIndex()) {
                drawer.setTextureID(textureIds[idx])
            }
        }

        private fun configWordSize() {
            mDrawers.forEach { it.setWorldSize(mWidth, mHeight) }
        }

        private fun destroyEGLSurface() {
            mEGLSurface?.destroyEGLSurface()
            mHaveBindEGLContext = false
        }

        private fun releaseEGL() {
            mEGLSurface?.release()
        }

        private fun render() {
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)
            mDrawers.forEach { it.draw() }
            mEGLSurface?.swapBuffers()
        }

        private fun createEGLSurface() {
            mEGLSurface?.createEGLSurface(mSurfaceView?.get()?.holder?.surface)
            mEGLSurface?.makeCurrent()
        }
    }

    /**
     * 渲染状态
     */
    enum class RenderState {
        NO_SURFACE, //没有有效的surface
        FRESH_SURFACE, //持有一个未初始化的新的surface
        SURFACE_CHANGE, //surface尺寸变化
        RENDERING, //初始化完毕，可以开始渲染
        SURFACE_DESTROY, //surface销毁
        STOP //停止绘制
    }

}
package com.pxy.liblarkar;

import android.app.Activity;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;

import com.google.ar.core.ArCoreApk;
import com.google.ar.core.exceptions.UnavailableDeviceNotCompatibleException;
import com.google.ar.core.exceptions.UnavailableUserDeclinedInstallationException;
import com.huawei.hiar.AREnginesApk;
import com.pxy.cloudlarkxrkit.XrSystem;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ArActivity extends Activity {
    private static final String TAG = ArActivity.class.getSimpleName();

    public static final int AR_SDK_TYPE_ARCORE = 1;
    public static final int AR_SDK_TYPE_HW_ARENGINE = 2;

    private static final double ROTATE_ANG_VEC = Math.PI / 100.0f;

    private GLSurfaceView mSurfaceView;

    private DisplayRotationManager mDisplayRotationManager;

    private boolean isRemindInstall = false;

    // lark xr system
    private XrSystem mXrSystem = null;

    private long mNativeApplication = 0;

    //姿态+点击事件
    private GestureDetector mGestureDetector;
    private ScaleGestureDetector mScaleGestureDetector;

    private int frames = 0;
    private long lastInterval;
    private float fps;

    private double mRotateRadius = 0.0f;
    private float mScaleFactor = 1.f;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        PermissionManager.checkPermission(this);

        setContentView(R.layout.activity_ar);

        mSurfaceView = findViewById(R.id.surfaceview);
        Button back= findViewById(R.id.back);
        back.setOnClickListener(view -> doBack());

        mXrSystem = new XrSystem();
        mXrSystem.init(this);

        String appid = getIntent().getStringExtra("appid");
//        String appid = "957963652122542080";
        int sdkType = getIntent().getIntExtra("arSDK", -1);

        if (sdkType == -1) {
            Log.w(TAG, "wrong sdk type " + sdkType + " force use default " + AR_SDK_TYPE_ARCORE);
            sdkType = AR_SDK_TYPE_ARCORE;
        }

        Log.d(TAG, "on create " + appid + " sdktype " + sdkType);
        mNativeApplication = JniInterface.createNativeApplication(this, appid, sdkType);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        //显示旋转管理器
        mDisplayRotationManager = new DisplayRotationManager(this);
        //初始姿态检测器
        initGestureDetector();
        //设置暂停时保留EGL上下文
        mSurfaceView.setPreserveEGLContextOnPause(true);
        //设置EGL上下文客户端版本
        mSurfaceView.setEGLContextClientVersion(2);
        //设置EGL配置选择器，包括颜色缓冲区的位数和深度位数。
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        JniInterface.setAssetManager(getAssets());

        mSurfaceView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
                Log.i(TAG,"onGlSurfaceCreated");
                JniInterface.onGlSurfaceCreated(mNativeApplication);
                showJniToast("寻找平面中，点击平面开始连接云端渲染");
            }

            @Override
            public void onSurfaceChanged(GL10 gl10, int width, int height) {
                mDisplayRotationManager.updateViewportRotation(width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl10) {
                // Synchronized to avoid racing onDestroy.
                synchronized (this) {
                    if (mNativeApplication == 0) {
                        return;
                    }
                    if (mDisplayRotationManager.getDeviceRotation()) {
                        mDisplayRotationManager.updateArSessionDisplayGeometry(mNativeApplication);
                    }
                    JniInterface.onGlSurfaceDrawFrame(mNativeApplication);

                    doFpsCalculate();
                }
            }
        });
        //连续渲染模式
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        mSurfaceView.setWillNotDraw(false);

        ArCoreApk.Availability availability = ArCoreApk.getInstance().checkAvailability(this);
        Log.d(TAG, "availablity arcore " + availability.isTransient() + " " + availability.isSupported() + " " + availability.isUnknown());

        try {
            ArCoreApk.InstallStatus installStatus = ArCoreApk.getInstance().requestInstall(this, false);
            Log.d(TAG, "availablity arcore install status " + installStatus);
        } catch (UnavailableDeviceNotCompatibleException e) {
            e.printStackTrace();
        } catch (UnavailableUserDeclinedInstallationException e) {
            e.printStackTrace();
        }

        AREnginesApk.ARAvailability hw = AREnginesApk.checkAvailability(this);
        Log.d(TAG, "availablity arengine " + hw.isTransient() + " " + hw.isSupported() + " " + hw.isUnknown());
        AREnginesApk.ARInstallStatus hwInstall = AREnginesApk.requestInstall(this, false);

        Log.d(TAG, "availablity arengine install status " + hwInstall);
    }

    private void initGestureDetector() {
        //姿态检测器
        mGestureDetector = new GestureDetector(this,
                new GestureDetector.SimpleOnGestureListener() {
                    @Override
                    public boolean onSingleTapUp(MotionEvent e) {
                        Log.d(TAG, "onSingleTapUp");
                        mSurfaceView.queueEvent(() ->
                                JniInterface.onTouched(mNativeApplication, e.getX(), e.getY(),false));
                        return true;
                    }

                    @Override
                    public void onLongPress(MotionEvent e) {
                        Log.d(TAG, "onLongPress");
                        mSurfaceView.queueEvent(() ->
                                JniInterface.onTouched(mNativeApplication, e.getX(), e.getY(),true));
                    }

                    @Override
                    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
                        float direction = distanceX > 0 ? 1 : -1;
                        mRotateRadius += ROTATE_ANG_VEC * direction;
                        Log.v(TAG, "onScroll X=" + distanceX + " Y=" + distanceY + " R=" + Math.toDegrees(mRotateRadius));
                        JniInterface.setRotateRadius(mNativeApplication, (float)mRotateRadius);
                        return super.onScroll(e1, e2, distanceX, distanceY);
                    }
                });

        mScaleGestureDetector = new ScaleGestureDetector(this, new ScaleGestureDetector.SimpleOnScaleGestureListener() {
            @Override
            public boolean onScale(ScaleGestureDetector detector) {
                float factor = 1;
                float speed = 1;
                if (detector.getScaleFactor() >= 1) {
                    factor = (detector.getScaleFactor() - 1) * 0.1f + 1;
                    speed = 0.05f;
                } else {
                    factor = detector.getScaleFactor();
                    speed = -0.05f;
                }
                mScaleFactor += mScaleFactor > 1 ? speed * factor : speed * factor * 0.1;

                // Don't let the object get too small or too large.
                mScaleFactor = Math.max(1.0f, Math.min(mScaleFactor, 8.0f));
                Log.v(TAG,  "on scale " + mScaleFactor + " " + detector.getScaleFactor());
                JniInterface.setScaleFactor(mNativeApplication, mScaleFactor);
                return super.onScale(detector);
            }
        });

        mSurfaceView.setOnTouchListener((v, event) -> {
            boolean res = mScaleGestureDetector.onTouchEvent(event);
            if (!mScaleGestureDetector.isInProgress()) {
                mGestureDetector.onTouchEvent(event);
            }
            return res;
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        JniInterface.onResume(mNativeApplication, getApplicationContext(), this);
        mSurfaceView.onResume();
        // Listen to display changed events to detect 180 rotation, which does not cause config change or view resize.
        mDisplayRotationManager.registerDisplayListener();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!PermissionManager.hasPermission(this)) {
            Toast.makeText(this, "This application needs camera permission.", Toast.LENGTH_LONG).show();
            finish();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        JniInterface.onPause(mNativeApplication);
        mDisplayRotationManager.unregisterDisplayListener();
        mSurfaceView.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mXrSystem.onDestroy();

        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            JniInterface.destroyNativeApplication(mNativeApplication);
            mNativeApplication = 0;
        }
    }

    @Override
    public void onWindowFocusChanged(boolean isHasFocus) {
        super.onWindowFocusChanged(isHasFocus);
        if (isHasFocus) {
            getWindow().getDecorView()
                    .setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }

    public void showJniToast(String msg) {
        runOnUiThread(() -> Toast.makeText(this, msg, Toast.LENGTH_SHORT).show());
    }

    private void doBack(){
        AlertDialog.Builder builder;
        builder = new AlertDialog.Builder(this).setTitle("退出应用")
                .setMessage("确定退出么").setPositiveButton("确定",
                        (dialogInterface, i) -> finish()).setNegativeButton("取消", (dialogInterface, i) -> dialogInterface.dismiss());

        builder.create().show();
    }

    @Override
    public void onBackPressed() {
        doBack();
    }

    private float doFpsCalculate() {
        ++frames;
        long timeNow = System.currentTimeMillis();

        // Convert millisecond to second.
        if (((timeNow - lastInterval) / 1000.0f) > 0.5f) {
            fps = frames / ((timeNow - lastInterval) / 1000.0f);
            frames = 0;
            lastInterval = timeNow;
            Log.i(TAG, "FPS " + fps);
        }
        return fps;
    }
}

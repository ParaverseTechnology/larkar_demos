package com.pxy.larkar_h5ui;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.webkit.ConsoleMessage;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.google.ar.core.ArCoreApk;
import com.huawei.hiar.AREnginesApk;
import com.pxy.cloudlarkxrkit.Config;
import com.pxy.liblarkar.ArActivity;
import com.pxy.liblarkar.PermissionManager;
import com.pxy.liblarkvr.VrActivity;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();
    private WebView mWebView;

    private int mSelectArSDK = ArActivity.AR_SDK_TYPE_ARCORE;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mWebView = findViewById(R.id.activity_main_webview);

        // Enable Javascript
        WebSettings webSettings = mWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);// 支持JS
        webSettings.setCacheMode(WebSettings.LOAD_DEFAULT); //设置缓存
        webSettings.setJavaScriptCanOpenWindowsAutomatically(true);//支持通过js打开新的窗口
        webSettings.setBuiltInZoomControls(false);// 设置支持缩放
        webSettings.setDomStorageEnabled(true);//使用localStorage则必须打开
        webSettings.setSupportMultipleWindows(false);// 设置同一个界面
        webSettings.setNeedInitialFocus(false);// 禁止webview上面控件获取焦点(黄色边框)
        webSettings.setDatabaseEnabled(true);
        webSettings.setAppCacheEnabled(true);
        webSettings.setAllowFileAccess(true);
        webSettings.setSupportZoom(true);
        webSettings.setLayoutAlgorithm(WebSettings.LayoutAlgorithm.NARROW_COLUMNS);
        webSettings.setUseWideViewPort(true);

        webSettings.setMediaPlaybackRequiresUserGesture(false); // 自动播放视频
        webSettings.setAllowUniversalAccessFromFileURLs(true);

        // LOCAL RESOURCE
        mWebView.loadUrl("file:///android_asset/web/index.html");

//        mWebView.clearHistory();
//        mWebView.clearCache(true);
//        mWebView.clearFormData();
//        mWebView.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);// 隐藏滚动条webView.requestFocus();
//        mWebView.requestFocusFromTouch();

        //浏览器中调试
        WebView.setWebContentsDebuggingEnabled(true);

        mWebView.addJavascriptInterface(new JsBridge(), "$Native");

        mWebView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onConsoleMessage(ConsoleMessage consoleMessage) {
                Log.d("WebView", consoleMessage.message());
                return super.onConsoleMessage(consoleMessage);
            };
        });

        // check permisssion
        PermissionManager.checkPermission(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    class JsBridge {
        @JavascriptInterface
        public void showToast(String msg) {
            runOnUiThread(() -> {
                Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();
            });
        }
        @JavascriptInterface
        public void enterAppli(String appid, boolean isVR) {
            runOnUiThread(() -> {
                if (isVR) {
                    Intent intent = new Intent(MainActivity.this, VrActivity.class);
                    intent.putExtra("appid", appid);
                    startActivity(intent);
                } else {
                    Intent intent = new Intent(MainActivity.this, ArActivity.class);
                    intent.putExtra("appid", appid);
                    intent.putExtra("arSDK", mSelectArSDK);
                    startActivity(intent);
                }
            });
        }
        @JavascriptInterface
        public void onSaveServerAddress(String ip, int port) {
            runOnUiThread(() -> {
                Log.i(TAG, "save server address " + ip + " " + port);
                Config config = Config.readFromCache(MainActivity.this);
                config.serverIp = ip;
                config.serverPort = port;
                Config.saveToCache(MainActivity.this, config);
            });
        }
        @JavascriptInterface
        public boolean hasPermission() {
            Log.i(TAG, "hasPermission");
            return PermissionManager.hasPermission(MainActivity.this);
        }
        @JavascriptInterface
        public void checkPermission() {
            Log.i(TAG, "checkPermission");
            PermissionManager.checkPermission(MainActivity.this);
        }

        @JavascriptInterface
        public int arcoreSupportStatus() {
            ArCoreApk.Availability availability = ArCoreApk.getInstance().checkAvailability(MainActivity.this);
            Log.d(TAG, "availablity arcore " + availability.isTransient() + " " + availability.isSupported() + " " + availability.isUnknown());

            if (availability.isSupported()) {
                return 0;
            } else if (availability.isTransient()) {
                return 1;
            } else {
                return 2;
            }
        }

        @JavascriptInterface
        public int arcoreInstallStatus(boolean requireInstall) {
            Log.d(TAG, "arcoreInstallStatus requireInstall=" + requireInstall);
            // Ensure that Google Play Services for AR and ARCore device profile data are
            // installed and up to date.
            try {
                switch (ArCoreApk.getInstance().requestInstall(MainActivity.this, requireInstall)) {
                    case INSTALLED:
                        // Success: Safe to create the AR session.
                        Log.i(TAG, "arcore install success");
                        return 0;
                    case INSTALL_REQUESTED:
                        // When this method returns `INSTALL_REQUESTED`:
                        // 1. ARCore pauses this activity.
                        // 2. ARCore prompts the user to install or update Google Play
                        //    Services for AR (market://details?id=com.google.ar.core).
                        // 3. ARCore downloads the latest device profile data.
                        // 4. ARCore resumes this activity. The next invocation of
                        //    requestInstall() will either return `INSTALLED` or throw an
                        //    exception if the installation or update did not succeed.
                        Log.i(TAG, "arcore need install " + requireInstall);
                        return 1;
                }
            } catch (Exception e) {
                Log.i(TAG, "arcore install failed" + requireInstall);
                e.printStackTrace();
            }
            return 2;
        }

        @JavascriptInterface
        public int hwarengineSupportStatus() {
            AREnginesApk.ARAvailability hw = AREnginesApk.checkAvailability(MainActivity.this);
            Log.d(TAG, "availablity arengine " + hw.isTransient() + " " + hw.isSupported() + " " + hw.isUnknown());

            if (hw.isSupported()) {
                return 0;
            } else if (hw.isTransient()) {
                return 1;
            } else {
                return 2;
            }
        }

        @JavascriptInterface
        public int hwarengineInstallStatus(boolean require) {
            switch (AREnginesApk.requestInstall(MainActivity.this, require)) {
                case INSTALLED:
                    Log.i(TAG, "arengine install success");
                    return 0;
                case INSTALL_REQUESTED:
                    Log.i(TAG, "arengine need install " + require);
                    return 1;
            }
            Log.i(TAG, "arengine install failed" + require);
            return 2;
        }

        @JavascriptInterface
        public boolean selectARSDK(int sdkType) {
            if (sdkType != ArActivity.AR_SDK_TYPE_ARCORE && sdkType != ArActivity.AR_SDK_TYPE_HW_ARENGINE) {
                Log.w(TAG, "unsupport ar sdk type " + sdkType);
                return false;
            }
            mSelectArSDK = sdkType;
            Log.i(TAG, "selectArSDK " + mSelectArSDK);
            return true;
        }

        @JavascriptInterface
        public void selectQuickConfigLevel(int level) {
            runOnUiThread(() -> {
                Log.i(TAG, "selectQuickConfigLevel " + level);
                if (level < 0 || level > 3) {
                    return;
                }
                Config config = Config.readFromCache(MainActivity.this);
                config.quickConfigLevel = level;
                Config.saveToCache(MainActivity.this, config);
            });
        }

        @JavascriptInterface
        public boolean enableCloudXR() {
            return ArActivity.ENABLE_CLOUDXR;
        }
    }
}
/*
 * Copyright 2019 Google Inc. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

package com.pxy.liblarkar;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;

/** JNI interface to native layer. */
public class JniInterface {
  static {
    System.loadLibrary("larkar");
    // cloud xr sdk use JNI_OnLoad to get java vm;
    // muse load lib to trigger JNI_Onload
    if (BuildConfig.ENABLE_CLOUDXR) {
      System.loadLibrary("CloudXRClient");
    }
  }

  private static final String TAG = "JniInterface";
  private static AssetManager mAssetManager;

  public static Bitmap loadImage(String imageName) {
    try {
      return BitmapFactory.decodeStream(mAssetManager.open(imageName));
    } catch (IOException e) {
      Log.e(TAG, "Cannot open image " + imageName);
      return null;
    }
  }

  public static void loadTexture(int target, Bitmap bitmap) {
    GLUtils.texImage2D(target, 0, bitmap, 0);
  }

  public static void setAssetManager(@NonNull AssetManager tassetManager) {
    mAssetManager = tassetManager;
  }

  public static native long createNativeApplication(ArActivity arActivity, String appid, int sdkType);

  public static native void onResume(long nativeApplication, Context context, Activity activity);

  public static native void onPause(long nativeApplication);

  public static native void onDisplayGeometryChanged(long nativeApplication, int displayRotation, int width,
                                                     int height);

  public static native void onGlSurfaceDrawFrame(long nativeApplication);

  public static native void onGlSurfaceCreated(long nativeApplication);

  public static native void onTouched(long nativeApplication, float x, float y,boolean longtap);

  public static native void setRotateRadius(long nativeApplication, float rotateRadius);

  public static native void setScaleFactor(long nativeApplication, float scaleFactor);

  public static native void destroyNativeApplication(long nativeApplication);
}

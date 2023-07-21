# LarkXR-AR

---

[English](./README.md) [中文](./README.zh_CN.md)

---

## LarkXR-AR demos

Show how to use [LarkXR SDK](https://github.com/pingxingyun/larkar_demos/releases) with arcore and hw arengine build cloudrendering AR app.

Include:

Client demo: android app with arcore and hw arengine buildin show cloudrendering AR.

Cloud 3D app demo: Unity3D with steamvr plugin, Unreal4.27 with steamvr plugin.

```
phone
-------------------                 ----------           ----------
|                  |                |        |           |         |
| arsdk(arcore...) |   streaimg     | LarkXR |  manange  |  XR App |
| larkxrsdk        | <=============>| Server |<=========>| OpenVR  |
|                  |  tracked pose  |        |           |         |  
-------------------                 ----------           -----------
```

Client send tracked pose to cloud app.

LarkXR Render Server steaming video to client.

LarkXR Server select apptype `AR` when upload.

## [AR Android client](https://github.com/pingxingyun/larkar_demos/tree/main/client_android_native)

* With [google arcore](https://developers.google.com/ar) support.
* With [huawei arengine sdk](https://developer.huawei.com/consumer/cn/hms/huawei-arengine/) support.
* [LarkXR SDK](https://github.com/pingxingyun/larkar_demos/tree/main/client_android_native/larkar_android/lib_larkar/libs)
* Option CloudXR, ee [CloudXR](./client_android_native/)
* LarkSR webclient with webview.

More [./client_android_native/README.md](./client_android_native/README.md)

![AR](./docs/unity2.jpg)

## [Cloud Unity Demo](https://github.com/pingxingyun/larkar_demos/tree/main/cloudapp_unity)

* Enable OpenVR plguin.
* Auto start play animation
* Play animation control by client.

More [./cloudapp_unity/README.md](./cloudapp_unity/README.md)

## [Cloud Unreal Demo](https://github.com/pingxingyun/larkar_demos/tree/main/cloudapp_unreal)

* Enable OpenVR

More[./cloudapp_unreal/README.md](./cloudapp_unreal/README.md)

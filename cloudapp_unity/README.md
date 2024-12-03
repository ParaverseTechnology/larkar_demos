# Unity Project Package to Windows (AR) 

---

[English](./README.md) [中文](./README.zh_CN.md)

---

1. In your Unity3D project, Go to "File" >> click "Build Settings"

![](./doc/build_en.png)

2. Click "Player Settings"

![](./doc/player_en.png)

3. Scroll down to XR Setting >> Add OpenVR  >> after loading, back to the "Build Settings"

![](./doc/1.png)

4. Configure the Main Camera in Scene, delete the "Skybox", background setting as rgba(0,0,0,0)

![](./doc/2.png)

5. Select Windows Platform >> click "Add Open Scenes" >> click "Build"

![](./doc/package.png)

6. Compress the all those build files as a ZIP folder.

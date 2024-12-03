# Unreal Project Package to Windows (AR)

---

[English](./README.md) [中文](./README.zh_CN.md)

---

To make your Unreal Engine app be compatible with LarkXR AR Streaming, it has to integrate with OpenVR (SteamVR) plugin, it avaliable only at UE4, below are the steps under UE4, please also reference to Set Up a [Standing Camera for SteamVR](https://dev.epicgames.com/documentation/en-us/unreal-engine/set-up-a-standing-camera-for-steamvr?application_version=4.27)

1. In your Unreal Engine project, Go to [Edit] >> click [Plugins]

![](./doc/plugin_en.PNG)

2. Make sure [SteamVR plugin] is enabled

![](./doc/steamvr_en.PNG)

3. Go to [Edit] >> [Project Settings]

![](./doc/project_en.PNG)

4. Go to [Description] >> [Settings] >> makes sure the [Start in VR] is enabled.

![](./doc/startvr_en.PNG)

5. On the [Content Browser] tab, click [Add / Import] >> [Blueprint Class] >> click [Pawn]

![](./doc/pawn_en.PNG)

6. Double-click that newly created VRPawn >> under the [Components] tab, create a new scene and new camera.

![](./doc/vrcamera_en.PNG)

7. In the [Event Graph], create a [Set Tracking Origin], select [Floor Level]

![](./doc/origion_1.png)

![](./doc/origion_2.png)

8. Drag that [VRPawn] into the scene, in [Detail], configure the [Location] as x=0.0 y=0.0 z=0.0 and [Auto Possess Player] as [Player 0]

![](./doc/set_origion_en.png)

9. In the [World Outliner] tab, find [BP_Sky_Sphere],  enable [Actor Hidden In Game]

![](./doc/sky_en.png)

10. Go to [Edit] >> [Project Settings] >> search "alpha", in [Postprocessing], select [Allow through tonemapper]

![](./doc/alpha_en.png)

11. click "Platforms" > "Windows" > Check "Shipping" > click "Package Project"

![](./doc/package_en.png)

12. Compress the package folder as a ZIP folder. 
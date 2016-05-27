--------------------------------------------------------------
2016/05/17 - HUGVRCam - version 1.0 Beta Release
--------------------------------------------------------------

1) Before using, access folder "RunMe First", and run "Register.bat" to install the required HUGVRCam virtual cam
2) Inside unity, attach to the main camera the following component:
 HUGVRCam\Scripts\HUGVRCam.cs
The script will handle the 360 camera setup and camera bypassing to the browser

>>Important to run image effects: make sure this component is the first in component list inside camera object

3) Hit Play, now unity will stream whatever being rendered as a webcamera. You can use it in browser or any video capture program via the new camera HUGVRCam 

Tested Platforms:
-----------------
- Tested on Windows 64 bits
- Tested using Unity 64 bits

Known bugs:
-----------
- On Firefox, when closing the browser, the browser will remain running in the background. Needs to be killed using TaskManager

-----------------
MHD Yamen Saraiji
mrayyamen@gmail.com

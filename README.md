UnityCam
========
![UnityCam](http://myamens.com/Uploads/UnityCam/UnityCam.gif)

This plugin provides a way for unity3D applications and games to output the viewport to an external application and use the viewport as a webcamera for such applications.

For example: Recording viewport in a webcamera capture software (Sharpcap), or using it in browser applications (tested in Chrome).

Tested Platforms:
-----------------
- Tested on Windows 64 bits
- Tested using Unity 32,64 bits

For Skype, please use x32 version of the service found in "RunMe First" folder, check instructions bellow for how to run it.

Usage
=====

1) Before using, access folder "RunMe First", access the target platform folder (x32,x64) which depends on the application you will use to view the virtual camera in. Right click on "Register.bat" and choose "Run as Administrator" to register UnityCam plugin in Windows. A window will appear to confirm that the plugin was successfully registered.

2) Inside unity, attach to the main camera the following component:
 UnityCam\Scripts\UnityCam.cs

3) Hit Play, now unity will stream whatever being rendered as a webcamera. You can use it in browser or any video capture program via the new camera UnityCam.

Check the included sample scene for a fully working example.

Build
=====
Make sure to set Architecture to x86_64 in the Build Settings in order to get the plugin running when deployed.


UPDATE 2019/November
====================
Thanks to @ArmainAP, this plugin now supports UE4. Plugin for UE4 is located under "UnrealCamDemo" folder.

License
-------

Copyright (c) 2016 MHD Yamen Saraiji

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

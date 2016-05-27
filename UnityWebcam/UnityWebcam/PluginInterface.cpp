//
// Funnel - Syphon server plugin for Unity
//
// Copyright (C) 2013-2016 Keijiro Takahashi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "stdafx.h"
#include "IUnityInterface.h"
#include "IUnityGraphics.h"

namespace
{
    IUnityInterfaces* s_interfaces;
    IUnityGraphics* s_graphics;
}

extern "C"
{
	void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
	{}
    // Plugin load event
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* interfaces)
    {
        s_interfaces = interfaces;
        s_graphics = s_interfaces->Get<IUnityGraphics>();
        
        s_graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
        
        // Run OnGraphicsDeviceEvent(initialize) manually on plugin load.
        OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
    }
    
    // Plugin unload event
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
    {
        s_graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
    }
    
}

/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#ifndef MOLTEN_GRAPHICS_RENDERTARGET_HPP
#define MOLTEN_GRAPHICS_RENDERTARGET_HPP

#include "Molten/Graphics/Build.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Platform.hpp"

namespace Molten
{

    /** Render target specific for win32 systems. */
    class MOLTEN_GRAPHICS_API RenderTarget
    {

    public:

        RenderTarget() = default;
        virtual ~RenderTarget() = default;

        /** Get render target dimensions. */
        [[nodiscard]] virtual Vector2ui32 GetSize() const = 0;

    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

        /** Get win32 window handle. Only available on Windows. */
        [[nodiscard]] virtual HWND GetWin32Window() = 0;

        /** Get win32 device context handle. Only available on Windows. */
        [[nodiscard]] virtual HDC GetWin32DeviceContext() = 0;

        /** Get win32 instance handle. Only available on Windows.*/
        [[nodiscard]] virtual HINSTANCE GetWin32Instance() = 0;

    #elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

        /** Get X11 display Device. Only available on Linux. */
        [[nodiscard]] virtual ::Display* GetX11DisplayDevice() = 0;

        /** Get X11 window Device. Only available on Linux. */
        [[nodiscard]] virtual ::Window GetX11WindowDevice() = 0;

        /** Get X11 screen Device. Only available on Linux. */
        [[nodiscard]] virtual int GetX11ScreenDevice() = 0;

    #endif

    };

}

#endif
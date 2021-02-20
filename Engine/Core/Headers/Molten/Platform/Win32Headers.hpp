/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_PLATFORM_WIN32HEADERS_HPP
#define MOLTEN_CORE_PLATFORM_WIN32HEADERS_HPP

#include "Molten/Core.hpp"

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include <Windows.h>
#include <Windowsx.h>

#define MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI NTDDI_VERSION >= NTDDI_WINBLUE
#if(MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)
#include <shellscalingapi.h>
#endif

// Getting rid of conflicting Windows macros.
#ifdef max
    #undef max
#endif
#ifdef min
    #undef min
#endif
#ifdef DeleteFile
    #undef DeleteFile
#endif
#ifdef far
    #undef far
#endif
#ifdef near
    #undef near
#endif

#ifdef GetMessage
    #undef GetMessage
#endif
#ifdef CreateSemaphore
    #undef CreateSemaphore
#endif
#ifdef LoadImage
    #undef LoadImage
#endif


// Getting rid of conflicting Windows X macros.
#ifdef IsMinimized
    #undef IsMinimized
#endif
#ifdef IsMaximized
    #undef IsMaximized
#endif


#endif

#endif

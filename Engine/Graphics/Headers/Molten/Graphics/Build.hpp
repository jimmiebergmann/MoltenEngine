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

#ifndef MOLTEN_GRAPHICS_BUILD_HPP
#define MOLTEN_GRAPHICS_BUILD_HPP

#include "Molten/Core.hpp"

/*
* DLL import or export, required for Windows, example:
*   class MOLTEN_GRAPHICS_API MyClass {};
*/
#define MOLTEN_GRAPHICS_API

// Export API
#if !defined(MOLTEN_GRAPHICS_STATIC)
#ifdef _MSC_VER
#pragma warning(disable : 4251) // identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4100) // identifier' : unreferenced formal parameter
#endif

// Define as export or import, if MOLTEN_GRAPHICS_EXPORT is defined.
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
#undef MOLTEN_GRAPHICS_API
#if defined(MOLTEN_GRAPHICS_EXPORT)
#define MOLTEN_GRAPHICS_API __declspec(dllexport)
#else
#define MOLTEN_GRAPHICS_API __declspec(dllimport)
#endif
#endif
#endif

#endif

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

#include "Curse/Window/WindowBase.hpp"

#if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
#include "Curse/Window/WindowWin32.hpp"
#elif CURSE_PLATFORM == CURSE_PLATFORM_LINUX
#include "Curse/Window/WindowX11.hpp"
#endif  

namespace Curse
{

    WindowBase* WindowBase::Create()
    {
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        return new WindowWin32;
    #elif CURSE_PLATFORM == CURSE_PLATFORM_LINUX
        return new WindowX11;
    #else
        return nullptr
    #endif    
    }

    WindowBase* WindowBase::Create(const std::string& title, const Vector2ui32 size)
    {
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        return new WindowWin32(title, size);
    #elif CURSE_PLATFORM == CURSE_PLATFORM_LINUX
        return new WindowX11(title, size);
    #else
        return nullptr
    #endif    
    }

    WindowBase::~WindowBase()
    {
    }

}
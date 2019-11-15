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

#include "Curse/Window/WindowX11.hpp"

#if defined CURSE_PLATFORM_LINUX

namespace Curse
{

    WindowX11::WindowX11()
    {
    }

    WindowX11::WindowX11(const std::string& title, const Vector2ui32 size) :
        WindowX11()
    {
        Open(title, size);
    }

    WindowX11::~WindowX11()
    {
    }

    void WindowX11::Open(const std::string& /*title*/, const Vector2ui32 /*size*/)
    {       
    }

    void WindowX11::Close()
    { 
    }

    void WindowX11::Update()
    {
    }

    void WindowX11::Show(const bool /*show*/)
    {
    }

    void WindowX11::Hide()
    {
    }

    bool WindowX11::IsOpen() const
    {
        return false;
    }

}

#endif
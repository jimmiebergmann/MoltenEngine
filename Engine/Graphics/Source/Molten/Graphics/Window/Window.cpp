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

#include "Molten/Graphics/Window/Window.hpp"

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
#include "Molten/Graphics/Window/WindowWin32.hpp"
#elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
#include "Molten/Graphics/Window/WindowX11.hpp"
#endif  

namespace Molten
{

    // Window implementations.
    std::unique_ptr<Window> Window::Create([[maybe_unused]] const WindowDescriptor& descriptor)
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        auto window = std::make_unique<WindowWin32>();
        if (!window->Open(descriptor))
        {
            return nullptr;
        }
        return window;
    #elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
        auto window = std::make_unique<WindowX11>();
        if (!window->Open(descriptor))
        {
            return nullptr;
        }
        return window;
    #else
        return nullptr;
    #endif    
    }


    // Platform window implementations.
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
    void PlatformWindow::Message(const Type type, const std::string& title, const std::string& content)
    {
        UINT icon = type == Type::Error ? MB_ICONEXCLAMATION : MB_ICONINFORMATION;
        MessageBox(NULL, content.c_str(), title.c_str(), icon | MB_OK);
    }

    bool PlatformWindow::MessageConfirm(const Type type, const std::string& title, const std::string& content)
    {
        UINT icon = type == Type::Error ? MB_ICONEXCLAMATION : MB_ICONINFORMATION;
        int result = MessageBox(NULL, content.c_str(), title.c_str(), icon | MB_YESNO);
        return result == IDYES;
    }
#else
    void PlatformWindow::Message(const Type, const std::string&, const std::string&)
    {
    }

    bool PlatformWindow::MessageConfirm(const Type, const std::string&, const std::string&)
    {
        return false;
    }
#endif    

    
}
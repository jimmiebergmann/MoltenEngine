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

#ifndef CURSE_CORE_WINDOW_WINDOWX11_HPP
#define CURSE_CORE_WINDOW_WINDOWX11_HPP

#include "Curse/Window/Window.hpp"

#if CURSE_PLATFORM == CURSE_PLATFORM_LINUX

#include "Curse/Platform/X11Headers.hpp"
#include "Curse/Memory/Reference.hpp"

namespace Curse
{

    /**
    * @brief Window class of X11 application windows.
    */
    class CURSE_API WindowX11 : public Window
    {

    public:

        /**
        * @brief Constructor.
        */
        WindowX11();

        /**
        * @brief Constructs and opens window.
        */
        WindowX11(const std::string& title, const Vector2ui32 size, Logger* logger = nullptr);

        /**
        * @brief Destructor.
        */
        ~WindowX11();

        /**
        * @brief Open window.
        */
        virtual bool Open(const std::string& title, const Vector2ui32 size, Logger* logger = nullptr) override;

        /**
        * @brief Close window.
        */
        virtual void Close() override;

        /**
        * @brief Update window.
        */
        virtual void Update() override;

        /**
        * @brief Checks if window has been created and is open.
        *        An open window is not the same as "currently showing".
        */
        virtual bool IsOpen() const override;

        /**
        * @brief Checks if window is present and showing on screen.
        *        A showing window is not the same as "open".
        */
        virtual bool IsShowing() const override;

        /**
        * @brief Checks if window is maximized.
        */
        virtual bool IsMaximized() const override;

        /**
        * @brief Checks if window is minimized.
        *        Window is now minimized when closed.
        */
        virtual bool IsMinimized() const override;

        /**
        * @brief Show window.
        *
        * @param show Shows window if true, else hides window.
        * @param signal Signals OnShow if true. OnShow is not being signaled if current status equals to parameter show.
        */
        virtual void Show(const bool show = true, const bool signal = false) override;

        /**
        * @brief Hide window.
        *
        * @param signal Signals OnMaximize if true. OnShow is not being signaled if current status equals to parameter show.
        */
        virtual void Hide(const bool signal = false) override;

        /**
        * @brief Maximize window.
        *
        * @param signal Signals OnMaximize if true. OnMaximize is not being signaled if window already is maximized.
        */
        virtual void Maximize(const bool signal = false) override;

        /**
        * @brief Minimize window.
        *
        * @param signal Signals OnMinimize if true. OnMinimize is not being signaled if window already is minimized.
        */
        virtual void Minimize(const bool signal = false) override;

        /**
        * @brief Change current position of window.
        *
        * @param position New position of window.
        * @param signal Signals OnMove if true. OnMove is not being signaled if new position equals to current position.
        */
        virtual void Move(const Vector2i32& position, const bool signal = false) override;

        /**
        * @brief Change current size of window.
        *
        * @param size New size of window.
        * @param signal Signals OnResize if true. OnResize is not being signaled if new size equals to current size.
        */
        virtual void Resize(const Vector2ui32& size, const bool signal = false) override;

        /**
        * @brief Set current title of window.
        */
        virtual void SetTitle(const std::string& title) override;

        /**
        * @brief Get current DPI of window.
        */
        virtual Vector2ui32 GetDpi() const override;

        /**
        * @brief Get current scale of window.
        *        Same as GetDpi, but returning a floating point vector equal to DPI / 96.
        */
        virtual Vector2f32 GetScale() const override;

        /**
        * @brief Get current size of window.
        */
        virtual Vector2ui32 GetSize() const override;

        /**
        * @brief Get current position of window.
        */
        virtual Vector2i32 GetPosition() const override;

        /**
        * @brief Get title of window, being rendered in client area.
        */
        virtual std::string GetTitle() const override;

        /**
        * @brief Get user input of window.
        *        The method Update is being called and managed by the window.
        */
        virtual UserInput& GetUserInput() override;

        /**
        * @brief Get user input of window.
        *        The method Update is being called and managed by the window.
        */
        virtual const UserInput& GetUserInput() const override;

        /**
        * @brief Get X11 display Device.
        */
        virtual ::Display * GetX11DisplayDevice() const override;

        /**
        * @brief Get X11 window Device.
        */
        virtual ::Window GetX11WindowDevice() const override;

        /**
        * @brief Get X11 screen Device.
        */
        virtual int GetX11ScreenDevice() const override;

    private:

        Logger* m_logger;
        ::Display * m_display;
        int m_screen;
        ::Window m_window;
        bool m_open;

        bool m_showing;
        bool m_maximized;
        bool m_minimized;
        Vector2ui32 m_size;
        Vector2i32 m_position;
        std::string m_title;
        UserInput m_userInput;

    };

}

#endif

#endif

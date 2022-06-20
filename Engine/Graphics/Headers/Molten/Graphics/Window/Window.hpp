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

#ifndef MOLTEN_GRAPHICS_WINDOW_WINDOW_HPP
#define MOLTEN_GRAPHICS_WINDOW_WINDOW_HPP

#include "Molten/Graphics/RenderTarget.hpp"
#include "Molten/System/Signal.hpp"
#include "Molten/System/UserInput.hpp"
#include "Molten/Math/Vector.hpp"
#include <string>
#include <memory>
#include <functional>
#include <filesystem>

namespace Molten
{

    class Logger;
    class UserInput;


    /** Window creation descriptor. */
    struct MOLTEN_GRAPHICS_API WindowDescriptor
    {
        WindowDescriptor();

        std::string title;
        Vector2ui32 size;
        bool enableDragAndDrop;
        Logger* logger;
    };


    /** Base class of application windows. */
    class MOLTEN_GRAPHICS_API Window : public RenderTarget
    {

    public:

        /** Callback functions for file drops.*/
        using FilesDropEnterCallback = std::function<bool(std::vector<std::filesystem::path>&)>;
        using FilesDropMoveCallback = std::function<void(const Vector2i32&)>;
        using FilesDropLeaveCallback = std::function<void()>;
        using FilesDropCallback = std::function<void(std::vector<std::filesystem::path>&)>;

        /** Static function for creating a window object for the current platform.
         *  Window is being opened if parameters are passed.
         *
         * @return Pointer to window, nullptr if no window is available for the current platform.
         */
        static std::unique_ptr<Window> Create(const WindowDescriptor& descriptor);

        /** On DPI change signal. */
        Signal<Vector2ui32> OnDpiChange;

        /** On maximize signal. */
        Signal<Vector2i32> OnMaximize;

        /** On minimize signal. */
        Signal<Vector2i32> OnMinimize;

        /** On resize signal. */
        Signal<Vector2i32> OnMove;

        /** On resize signal. */
        Signal<Vector2ui32> OnResize;

        /** On scale change signal.
         *   Same as OnDpiChange, but returning a floating point vector equal to DPI / 96.
         */
        Signal<Vector2f32> OnScaleChange;

        /** On show/hide signal. */
        Signal<bool> OnShow;

        /** Function called when one or multiple files are dragged into window.
         *  Function should return true if any of the files are accepted, else false.
         *  This callback is only called if window is created with enableDragAndDrop set to true.
         */
        FilesDropEnterCallback OnFilesDropEnter;

        /** Function called when one or multiple files are moved into window.
          *  This callback is only called if window is created with enableDragAndDrop set to true.
          */
        FilesDropMoveCallback OnFilesDropMove;

        /** Function called when one or multiple files are moved out of window.
          *  This callback is only called if window is created with enableDragAndDrop set to true.
          */
        FilesDropLeaveCallback OnFilesDropLeave;

        /** Function called when one or multiple files are dropped into window.
         *  This callback is only called if window is created with enableDragAndDrop set to true.
         */
        FilesDropCallback OnFilesDrop;

        /** Default constructor. */
        Window() = default;

        /** Virtual destructor. */
        ~Window() override = default;

        /** Deleted copy and move constructors and operators. */
        /**@{*/
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator = (const Window&) = delete;
        Window& operator = (Window&&) = delete;
        /**@}*/

        /** Open window. */
        virtual bool Open(const WindowDescriptor& descriptor) = 0;

        /** Close window. */
        virtual void Close() = 0;

        /** Update window. */
        virtual void Update() = 0;

        /** Checks if window has been created and is open.
         *  An open window is not the same as "currently showing".
         */
        virtual bool IsOpen() const = 0;

        /** Checks if window is present and showing on screen.
         *  A showing window is not the same as "open".
         */
        virtual bool IsShowing() const = 0;

        /** Checks if window is maximized. */
        virtual bool IsMaximized() const = 0;

        /** Checks if window is minimized.
         *  Window is now minimized when closed.
         */
        virtual bool IsMinimized() const = 0;

        /** Checks if window is focused or not.
         *  A window is considered focused when it is selected and will receive user inputs.
         *  Minimizing or clicking on another desktop window will kill focus of this window and result in a return value of false.
         */
        virtual bool IsFocused() const = 0;

        /**
         * @brief Show window.
         *
         * @param show Shows window if true, else hides window.
         * @param signal Signals OnShow if true. OnShow is not being signaled if current status equals to parameter show.
         */
        virtual void Show(const bool show = true, const bool signal = false) = 0;

        /** Hide window.
         *
         * @param signal Signals OnMaximize if true. OnShow is not being signaled if current status equals to parameter show.
         */
        virtual void Hide(const bool signal = false) = 0;

        /** Maximize window.
         *
         * @param signal Signals OnMaximize if true. OnMaximize is not being signaled if window already is maximized.
         */
        virtual void Maximize(const bool signal = false) = 0;

        /** Minimize window.
         *
         * @param signal Signals OnMinimize if true. OnMinimize is not being signaled if window already is minimized.
         */
        virtual void Minimize(const bool signal = false) = 0;

        /** Change current position of window.
         *
         * @param position New position of window.
         * @param signal Signals OnMove if true. OnMove is not being signaled if new position equals to current position.
         */
        virtual void Move(const Vector2i32& position, const bool signal = false) = 0;

        /** Change current size of window.
         *
         * @param size New size of window.
         * @param signal Signals OnResize if true. OnResize is not being signaled if new size equals to current size.
         */
        virtual void Resize(const Vector2ui32& size, const bool signal = false) = 0;

        /** Set current title of window. */
        virtual void SetTitle(const std::string& title) = 0;

        /** Set current cursor.*/
        virtual void SetCursor(const Mouse::Cursor cursor) = 0;

        /** Get current DPI of window. */
        virtual Vector2ui32 GetDpi() const = 0;

        /** Get current scale of window.
         *   Same as GetDpi, but returning a floating point vector equal to DPI / 96.
         */
        virtual Vector2f32 GetScale() const = 0;

        /** Get current size of window. */
        virtual Vector2ui32 GetSize() const override = 0; 

        /** Get current position of window. */
        virtual Vector2i32 GetPosition() const = 0;

        /** Get title of window, being rendered in client area. */
        virtual std::string GetTitle() const = 0;

        /** Get user input of window.
         *  The method Update is being called and managed by the window.
         */
        virtual UserInput& GetUserInput() = 0;

        /** Get user input of window.
         *  The method Update is being called and managed by the window.
         */
        virtual const UserInput& GetUserInput() const = 0;

    };

    /**  Platform independent window class. */
    class MOLTEN_GRAPHICS_API PlatformWindow
    {

    public:

        /** Enumerator describing window type. */
        enum class Type
        {
            Info,
            Error
        };

        /** Show a modal window with a OK button. */
        static void Message(const Type type, const std::string& title, const std::string& content);

        /** Show a modal window with a Yes and No button.
         *
         * @return true if user pressed the Yes button, false if No button was pressed.
         */
        static bool MessageConfirm(const Type type, const std::string& title, const std::string& content);

    };

}

#endif

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

#ifndef CURSE_CORE_WINDOW_WINDOW_X11_HPP
#define CURSE_CORE_WINDOW_WINDOW_X11_HPP

#include "Curse/Window/WindowBase.hpp"

#if defined CURSE_PLATFORM_LINUX

#include <stdexcept>

namespace Curse
{

    /**
    * @brief Window class of X11 application windows.
    */
    class CURSE_API WindowX11 : public WindowBase
    {

    public:

        /**
        * @brief Constructor.
        */
        WindowX11();

        /**
        * @brief Constructs and opens window.
        */
        WindowX11(const std::string& title, const Vector2ui32 size);

        /**
        * @brief Destructor.
        */
        ~WindowX11();

        /**
        * @brief Open window.
        */
        virtual void Open(const std::string& title, const Vector2ui32 size);

        /**
        * @brief Close window.
        */
        virtual void Close();

        /**
        * @brief Update window.
        */
        virtual void Update();

        /**
        * @brief Show window.
        *
        * @param show Shows window if true, else hides window.
        */
        virtual void Show(const bool show = true);

        /**
        * @brief Hide window.
        */
        virtual void Hide();

        /**
        * @brief Checks if window has been created and is open.
        *        An open window is not the same as "currently showing".
        */
        virtual bool IsOpen() const;

    private:


    };

}

#endif

#endif

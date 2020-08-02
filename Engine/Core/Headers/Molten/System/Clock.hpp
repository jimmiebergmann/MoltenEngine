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

#ifndef MOLTEN_CORE_SYSTEM_CLOCK_HPP
#define MOLTEN_CORE_SYSTEM_CLOCK_HPP

#include "Molten/System/Time.hpp"

namespace Molten
{

    /**
    * @brief Clock class.
    *        It's not possible to stop or pause a clock.
    *        Clock starts to tick at construction.
    */
    class MOLTEN_API Clock
    {

    public:

        /**
        * @brief Constructor.
        *
        * @param startTime Initial time of clock.
        */
        Clock(const Time startTime = Time::Zero);

        /**
        * @brief Get current time of clock.
        */
        Time GetTime() const;

        /**
        * @brief Resets clock, sets initial time. 
        */
        void Reset(const Time startTime = Time::Zero);

    private:

        Time m_startTime;

    };

}

#endif

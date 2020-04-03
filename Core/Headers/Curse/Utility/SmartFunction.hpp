/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef CURSE_CORE_UTILITY_SMARTFUNCTION_HPP
#define CURSE_CORE_UTILITY_SMARTFUNCTION_HPP

#include "Curse/Types.hpp"
#include <functional>

namespace Curse
{

    /**
    * @brief Smart function class.
    *
    * This class makes it possible to call a function if an unhandled exeption is thrown.
    * The passed function will be called at destruction of this object.
    * Release the function before object destruction to prevent the function from being called at destruction.
    *
    * Example:
    *
    *   SmartFunction sf([this]()
    *   {
    *       // cleanup here..
    *   });
    *
    *   throw Exception("Error here.");
    *
    *   sf.Release();
    *
    */
    class CURSE_API SmartFunction
    {

    public:

        using Function = std::function<void()>; ///< Function data type.

        /**
        * @brief Constructor.
        *        Pass function to call at destruction.
        */
        SmartFunction(Function&& function);

        /**
        * @brief Destructor.
        *        Passed function is being called if Release hasn't been called once.
        */
        ~SmartFunction();

        /**
        * @brief Release the function. It's not going to be called at destruction anymore.
        */
        void Release();

    private:

        Function m_function;

    };

}

#endif
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

#ifndef MOLTEN_CORE_SYSTEM_RESULT_HPP
#define MOLTEN_CORE_SYSTEM_RESULT_HPP

#include "Molten/Core.hpp"
#include <variant>
#include <type_traits>

namespace Molten
{

    /* 
     * Result class with two possible values: Successful result and failed result.
     * Purpose of this object is to be used as return value for functions, 
     * with need of both error code value and requested data type.
     * Check if the result is successful via boolean operator or IsValid().
     * It is only possible to construct results via the static functions Success and Error,
     * or by moving results from one to another.
     */
    template<typename T, typename TError>
    class Result
    {

        static_assert(std::is_move_constructible<T>::value, "T is not move constructiable.");
        static_assert(std::is_move_constructible<TError>::value, "TError is not move constructiable.");

    public:

        using Type = T;             ///< Succeful return type of result.
        using ErrorType = TError;   ///< Error type of result.

        /* Creating a successful result. Access result value via Value(). */
        [[nodiscard]] static Result CreateSuccess(T&& type);

        /* Creating an error result. Access result value via Error(). */
        [[nodiscard]] static Result CreateError(TError&& error);

        /* Move constructor. */
        Result(Result&& result) noexcept;

        /* Move assigment operator. */
        Result& operator=(Result&& result) noexcept;

        /* Checks if result is successful or not. */
        /**@{*/
        [[nodiscard]] explicit operator bool() const;
        [[nodiscard]] bool IsValid() const;
        /**@}*/
   
        /* Get result error, accessing this method with a successful result is undefined behaivor. */
        /**@{*/
        [[nodiscard]] TError& Error();
        [[nodiscard]] const TError& Error() const;
        /**@}*/

        /* Get result value, accessing this method with an error result is undefined behaivor. */
        /**@{*/
        T& Value();
        const T& Value() const;
        
        T& operator *();
        const T& operator *() const;

        T* operator ->();
        const T* operator ->() const;
        /**@}*/

    private:

        using VariantType = std::variant<T, TError>;

        /* Private constructor. */
        explicit Result(VariantType&& variant);

        VariantType m_data;

    };

}

#include "Molten/System/Result.inl"

#endif
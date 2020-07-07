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

#ifndef CURSE_CORE_SYSTEM_RESULT_HPP
#define CURSE_CORE_SYSTEM_RESULT_HPP

#include "Curse/Core.hpp"
#include <variant>

namespace Curse
{

    /*
    * @breif 
    */
    template<typename T, typename E>
    class Result
    {

    public:

        using Type = T;         ///< Succeful return type of result.
        using ErrorType = E;    ///< Error type of result.

        /*
        * @brief Constructors.
        */
        /**@{*/
        [[nodiscard]] Result();

        [[nodiscard]] static Result Value(T&& type);

        [[nodiscard]] static Result Error(E&& error);
        /**@}*/

        /*
        * @brief Copy constructor.
        */
        [[nodiscard]] explicit Result(const Result& result);

        /*
        * @brief Move constructor.
        */
        [[nodiscard]] Result(Result&& result) noexcept;

        /*
        * @return true if result is successful, else false.
        */
        explicit operator bool() const;

        /*
        * @brief Copy assigment operator. 
        */
        Result& operator=(const Result& result);

        /*
        * @brief Move assigment operator.
        */
        Result& operator=(Result&& result) noexcept;
   
        /*
        * @brief Get result error, undefined behaivor if result is in succesful state.
        */
        /**@{*/
        [[nodiscard]] E& Error();
        [[nodiscard]] const E& Error() const;
        /**@}*/

        /*
        * @brief Get result value, undefined behaivor if result is in error state.
        */
        /**@{*/
        T& Value();
        const T& Value() const;
        /**@}*/

    private:

        /*
        * @brief Private constructor.
        */
        Result(std::variant<T, E>&& variant);

        using VariantType = std::variant<T, E>;

        VariantType m_data;

    };

}

#include "Curse/System/Result.inl"

#endif
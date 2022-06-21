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

#ifndef MOLTEN_CORE_UTILITY_FUNCTIONDISPATCHER_HPP
#define MOLTEN_CORE_UTILITY_FUNCTIONDISPATCHER_HPP

#include "Molten/Core.hpp"
#include <functional>
#include <vector>
#include <mutex>

namespace Molten
{

    class MOLTEN_CORE_API FunctionDispatcher
    {

    public:

        using Function = std::function<void()>; ///< Function data type.

        FunctionDispatcher() = default;
        ~FunctionDispatcher();

        FunctionDispatcher(const FunctionDispatcher&) = delete;
        FunctionDispatcher(FunctionDispatcher&&) = delete;
        FunctionDispatcher& operator = (const FunctionDispatcher&) = delete;
        FunctionDispatcher& operator = (FunctionDispatcher&&) = delete;

        void Add(Function&& function);
        void Dispatch();

    private:

        [[nodiscard]] size_t LockGetFunctionCount();
        [[nodiscard]] Function LockMoveFunction(const size_t index);
        void LockRemoveFunctions(const size_t count);

        std::vector<Function> m_functions;
        std::mutex m_mutex;

    };

}

#endif
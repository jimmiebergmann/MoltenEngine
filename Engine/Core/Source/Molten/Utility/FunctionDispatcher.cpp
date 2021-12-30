/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#include "Molten/Utility/FunctionDispatcher.hpp"

namespace Molten
{

    FunctionDispatcher::~FunctionDispatcher()
    {
        const std::lock_guard lock(m_mutex);
        m_functions.clear();
    }

    void FunctionDispatcher::Add(Function&& function)
    {
        const std::lock_guard lock(m_mutex);
        m_functions.push_back(std::move(function));
    }

    void FunctionDispatcher::Dispatch()
    {
        const auto count = LockGetFunctionCount();

        for(size_t i = 0; i < count; i++)
        {
            auto function = LockMoveFunction(i);
            function();
        }

        LockRemoveFunctions(count);
    }

    size_t FunctionDispatcher::LockGetFunctionCount()
    {
        const std::lock_guard lock(m_mutex);
        return m_functions.size();
    }

    FunctionDispatcher::Function FunctionDispatcher::LockMoveFunction(const size_t index)
    {
        const std::lock_guard lock(m_mutex);
        return std::move(m_functions[index]);
    }

    void FunctionDispatcher::LockRemoveFunctions(const size_t count)
    {
	    if(count == 0)
	    {
            return;
	    }

        const std::lock_guard lock(m_mutex);
        m_functions.erase(m_functions.begin(), m_functions.begin() + count);
    }


}
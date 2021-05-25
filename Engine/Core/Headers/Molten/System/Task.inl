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

namespace Molten
{

    // Parallel task group implementations.
    template<typename TEntryType, typename ... TTaskArgs>
    std::shared_ptr<TEntryType> ParallelTaskGroup::Emplace(TTaskArgs ... taskArgs)
    {
        if constexpr (std::is_same_v<TEntryType, Task> == true)
        {
            auto entry = std::make_shared<TEntryType>(std::forward<TTaskArgs>(taskArgs)...);
            m_tasks.push_back(entry);
            return entry;
        }
        else
        {
            static_assert(false, "ParallelTaskGroup::Emplace: Provided TEntryType is not supported.");
        }
    }


    // Serial task group implementations.
    template<typename TEntryType, typename ... TTaskArgs>
    std::shared_ptr<TEntryType> SerialTaskGroup::EmplaceFront(TTaskArgs ... taskArgs)
    {
        if constexpr (std::is_same_v<TEntryType, Task> == true)
        {
            auto entry = std::make_shared<TEntryType>(std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(m_entries.begin(), entry);
            return entry;
        }
        else if constexpr(std::is_same_v<TEntryType, ParallelTaskGroup> == true)
        {
            auto entry = std::make_shared<TEntryType>(m_threadPool, std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(m_entries.begin(), entry);
            return entry;
        }
        else
        {
            static_assert(false, "SerialTaskGroup::EmplaceFront: Provided TEntryType is not supported.");
        }  
    }

    template<typename TEntryType, typename ... TTaskArgs>
    std::shared_ptr<TEntryType> SerialTaskGroup::EmplaceBack(TTaskArgs ... taskArgs)
    {
        if constexpr (std::is_same_v<TEntryType, Task> == true)
        {
            auto entry = std::make_shared<TEntryType>(std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(m_entries.end(), entry);
            return entry;
        }
        else if constexpr (std::is_same_v<TEntryType, ParallelTaskGroup> == true)
        {
            auto entry = std::make_shared<TEntryType>(m_threadPool, std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(m_entries.end(), entry);
            return entry;
        }
        else
        {
            static_assert(false, "SerialTaskGroup::EmplaceBack: Provided TEntryType is not supported.");
        }
    }

    template<typename TEntryType, typename ... TTaskArgs>
    std::shared_ptr<TEntryType> SerialTaskGroup::Emplace(Entries::const_iterator it, TTaskArgs ... taskArgs)
    {
        if constexpr (std::is_same_v<TEntryType, Task> == true)
        {
            auto entry = std::make_shared<TEntryType>(std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(it, entry);
            return entry;
        }
        else if constexpr (std::is_same_v<TEntryType, ParallelTaskGroup> == true)
        {
            auto entry = std::make_shared<TEntryType>(m_threadPool, std::forward<TTaskArgs>(taskArgs)...);
            m_entries.insert(it, entry);
            return entry;
        }
        else
        {
            static_assert(false, "SerialTaskGroup::Emplace: Provided TEntryType is not supported.");
        }
    }

}
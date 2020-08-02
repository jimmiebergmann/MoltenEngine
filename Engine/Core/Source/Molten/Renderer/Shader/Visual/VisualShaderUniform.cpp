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


#include "Molten/Renderer/Shader/Visual/VisualShaderUniform.hpp"

namespace Molten::Shader::Visual
{
    
    // Uniform meta data implementations.
    UniformMetaData::UniformMetaData(const uint32_t id) :
        m_id(id)
    {}

    uint32_t UniformMetaData::GetId() const
    {
        return m_id;
    }


    // Uniform interfaces container implementations.
    UniformInterfaces::UniformInterfaces(Script& script) :
        m_script(script)
    {}

    UniformInterfaces::~UniformInterfaces()
    {
        RemoveAllInterfaces();
    }
 
    UniformInterfaces::InterfaceContainer::iterator UniformInterfaces::begin()
    {
        return m_interfaces.begin();
    }
    UniformInterfaces::InterfaceContainer::const_iterator UniformInterfaces::begin() const
    {
        return m_interfaces.begin();
    }

    UniformInterfaces::InterfaceContainer::iterator UniformInterfaces::end()
    {
        return m_interfaces.end();
    }
    UniformInterfaces::InterfaceContainer::const_iterator UniformInterfaces::end() const
    {
        return m_interfaces.end();
    }

    UniformInterface* UniformInterfaces::AddInterface(const uint32_t id)
    {
        if (m_usedIds.find(id) != m_usedIds.end())
        {
            return nullptr;
        }

        auto* newInterface = new UniformInterface(m_script, id);
        m_interfaces.push_back(newInterface);
        m_usedIds.insert(id);
        return newInterface;
    }

    void UniformInterfaces::RemoveInterface(const size_t index)
    {
        if (index >= m_interfaces.size())
        {
            return;
        }
        auto it = m_interfaces.begin() + index;
        m_usedIds.erase((*it)->GetId());
        delete *it;
        m_interfaces.erase(it);
    }
    void UniformInterfaces::RemoveInterface(InterfaceContainer::iterator it)
    {
        m_interfaces.erase(it);
    }

    void UniformInterfaces::RemoveAllInterfaces()
    {
        for (auto* inter : m_interfaces)
        {
            delete inter;
        }
        m_interfaces.clear();
        m_usedIds.clear();
    }

    UniformInterface* UniformInterfaces::GetInterface(const size_t index)
    {
        if (index >= m_interfaces.size())
        {
            return nullptr;
        }
        return m_interfaces[index];
    }

    size_t UniformInterfaces::GetInterfaceCount() const
    {
        return m_interfaces.size();
    }

}
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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERUNIFORM_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERUNIFORM_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include <set>

namespace Molten::Shader::Visual
{

    //// Forward declarations.
    //class Script;
    //class UniformMetaData;


    ///** 
    // * Meta data related to uniform interfaces.
    // * The only available meta data for uniform interfaces is IDs.
    // */
    //class MOLTEN_API UniformMetaData
    //{

    //public:

    //    explicit UniformMetaData(const uint32_t id);

    //    uint32_t GetId() const;

    //private:

    //    /** Copy and move operations are not allowed. */
    //    /**@{*/
    //    UniformMetaData(const UniformMetaData& copy) = delete;
    //    UniformMetaData(const UniformMetaData&& move) = delete;
    //    UniformMetaData& operator =(const UniformMetaData& copy) = delete;
    //    UniformMetaData& operator =(const UniformMetaData&& move) = delete;
    //    /**@*/

    //    const uint32_t m_id;

    //};


    ///** Uniform interface type. */
    //using UniformInterface = Structure<InputVariable, void, UniformMetaData>;


    ///** Uniform interface container type. */
    //class MOLTEN_API UniformInterfaces
    //{

    //public:

    //    using InterfaceContainer = std::vector<UniformInterface*>;

    //    explicit UniformInterfaces(Script& script);
    //    ~UniformInterfaces();

    //    /** Interface iterators. */
    //    /**@{*/
    //    InterfaceContainer::iterator begin();
    //    InterfaceContainer::const_iterator begin() const;

    //    InterfaceContainer::iterator end();
    //    InterfaceContainer::const_iterator end() const;
    //    /**@}*/

    //    /** Append new interface. */
    //    UniformInterface* AddInterface(const uint32_t id);

    //    /**
    //     * Remove and destroy interface by index or iterator.
    //     * Accessing removed interfaces is undefined behaviour.
    //     */
    //    /**@{*/
    //    void RemoveInterface(const size_t index);
    //    void RemoveInterface(InterfaceContainer::iterator it);
    //    /**@}*/


    //    /** Remove all interfaces in this container. */
    //    void RemoveAllInterfaces();

    //    /** Get interface by index. Returns nullptr if index >= GetInterfaceCount(). */
    //    UniformInterface* GetInterface(const size_t index);

    //    /** Get number of interfaces in this container. */
    //    size_t GetInterfaceCount() const;
 
    //private:

    //    /** Copy and move operations are not allowed. */
    //    /**@{*/
    //    UniformInterfaces(const UniformInterfaces& copy) = delete;
    //    UniformInterfaces(const UniformInterfaces&& move) = delete;
    //    UniformInterfaces& operator =(const UniformInterfaces& copy) = delete;
    //    UniformInterfaces& operator =(const UniformInterfaces&& move) = delete;
    //    /**@*/

    //    Script& m_script;
    //    InterfaceContainer m_interfaces;
    //    std::set<uint32_t> m_usedIds;

    //};

}

#endif
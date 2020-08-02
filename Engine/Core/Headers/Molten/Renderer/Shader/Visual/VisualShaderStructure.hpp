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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSTRUCTURE_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSTRUCTURE_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderVariable.hpp"
#include <vector>

namespace Molten::Shader::Visual
{

    template<template<typename TDataType> typename TVariableType>
    class Structure
    {

    public:

        template<typename DataType>
        using VariableType = TVariableType<DataType>;
        using VariableBaseType = typename TVariableType<float>::Base;
        using VariableContainer = std::vector<VariableBaseType*>;
        using ConstVariableContainer = std::vector<const VariableBaseType*>;

        Structure(Script& script);
        ~Structure();

        template<typename DataType>
        VariableType<DataType>* AddMember();

        size_t GetMemberCount() const;

        template<typename DataType>
        VariableType<DataType>* GetMember(const size_t index);
        template<typename DataType>
        const VariableType<DataType>* GetMember(const size_t index) const;

        VariableBaseType* GetMember(const size_t index);
        const VariableBaseType* GetMember(const size_t index) const;

        VariableBaseType* operator[](const size_t index);
        const VariableBaseType* operator[](const size_t index) const;

        VariableContainer GetMembers();
        ConstVariableContainer GetMembers() const;

        template<template<typename OtherDataType> typename OtherVariableType>
        bool CheckCompability(const Structure<OtherVariableType>& other) const;

    private:

        Structure(const Structure& copy) = delete;
        Structure(const Structure&& move) = delete;
        Structure& operator =(const Structure& copy) = delete;
        Structure& operator =(const Structure&& move) = delete;

        Script& m_script;
        VariableContainer m_members;

        template<template<typename OtherDataType> typename OtherVariableType>
        friend class Structure;

    };

    using InputStructure = Structure<InputVariable>;
    using OutputStructure = Structure<OutputVariable>;

}

namespace Molten::Shader::Visual::Private
{

    template<typename TLeftBaseType, typename TRightBaseType>
    static bool CheckStructureCompability(const TLeftBaseType& lhs, const TRightBaseType& rhs);

}

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.inl"

#endif
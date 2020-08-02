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

    /**
     * Data structure container for visual shader scripts.
     * This container can be used for interface blocks for unifoms, vertex data or push constants.
     *
     * @see Script
     */
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

        /** Append new data member to this structure.  */
        template<typename DataType>
        VariableType<DataType>* AddMember();

        /** Get number of data members in this structure. */
        size_t GetMemberCount() const;

        /**
         * Get  data member by index and cast it to the currect variable type.
         * 
         * @param index must be less than GetMemberCount().
         */
        /**@{*/
        template<typename DataType>
        VariableType<DataType>* GetMember(const size_t index);
        template<typename DataType>
        const VariableType<DataType>* GetMember(const size_t index) const;
        /**@}*/

        /**
         * Get  data member by index.
         *
         * @param index must be less than GetMemberCount().
         */
        VariableBaseType* GetMember(const size_t index);
        const VariableBaseType* GetMember(const size_t index) const;

        /**
         * Operator for getting data member by index. Same as GetMember().
         *
         * @param index must be less than GetMemberCount().
         */
        VariableBaseType* operator[](const size_t index);
        const VariableBaseType* operator[](const size_t index) const;

        /**
         * Get all data members in this structure.
         * The returned container is not the original one, but a copy.
         */
        VariableContainer GetMembers();
        ConstVariableContainer GetMembers() const;

        template<template<typename OtherDataType> typename OtherVariableType>
        bool CheckCompability(const Structure<OtherVariableType>& other) const;

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        Structure(const Structure& copy) = delete;
        Structure(const Structure&& move) = delete;
        Structure& operator =(const Structure& copy) = delete;
        Structure& operator =(const Structure&& move) = delete;
        /**@*/

        Script& m_script; ///< parent script.
        VariableContainer m_members; ///< Container of data members.

        template<template<typename OtherDataType> typename OtherVariableType>
        friend class Structure;

    };

    using InputStructure = Structure<InputVariable>;
    using OutputStructure = Structure<OutputVariable>;

}

namespace Molten::Shader::Visual::Private
{

    /** Helper function for  */
    template<typename TLeftBaseType, typename TRightBaseType>
    static bool CheckStructureCompability(const TLeftBaseType& lhs, const TRightBaseType& rhs);

}

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.inl"

#endif
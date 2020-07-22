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

namespace Curse::Shader
{
    template<template<typename TDataType> typename TVariableType>
    Structure<TVariableType>::Structure(Script& script) :
        m_script(script)
    {}

    template<template<typename TDataType> typename TVariableType>
    Structure<TVariableType>::~Structure()
    {
        for (auto& member : m_members)
        {
            delete member;
        }
    }

    template<template<typename TDataType> typename TVariableType>
    template<typename DataType>
    typename Structure<TVariableType>::template VariableType<DataType>* Structure<TVariableType>::AddMember()
    {
        auto* member = new TVariableType<DataType>(m_script);
        m_members.push_back(member);
        return member;
    }

    template<template<typename TDataType> typename TVariableType>
    size_t Structure<TVariableType>::GetMemberCount() const
    {
        return m_members.size();
    }

    template<template<typename TDataType> typename TVariableType>
    template<typename DataType>
    typename Structure<TVariableType>::template VariableType<DataType>* Structure<TVariableType>::GetMember(const size_t index)
    {
        return static_cast<VariableType<DataType>*>(GetMember(index));
    }
    template<template<typename TDataType> typename TVariableType>
    template<typename DataType>
    const typename Structure<TVariableType>::template VariableType<DataType>* Structure<TVariableType>::GetMember(const size_t index) const
    {
        return static_cast<const VariableType<DataType>*>(GetMember(index));
    }
    
    template<template<typename TDataType> typename TVariableType>
    typename Structure<TVariableType>::VariableBaseType* Structure<TVariableType>::GetMember(const size_t index)
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }

    template<template<typename TDataType> typename TVariableType>
    const typename Structure<TVariableType>::VariableBaseType* Structure<TVariableType>::GetMember(const size_t index) const
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }

    template<template<typename TDataType> typename TVariableType>
    typename Structure<TVariableType>::VariableBaseType* Structure<TVariableType>::operator[](const size_t index)
    {
        return GetMember(index);
    }
    template<template<typename TDataType> typename TVariableType>
    const typename Structure<TVariableType>::VariableBaseType* Structure<TVariableType>::operator[](const size_t index) const
    {
        return GetMember(index);
    }

    template<template<typename TDataType> typename TVariableType>
    typename Structure<TVariableType>::VariableContainer Structure<TVariableType>::GetMembers()
    {
        return { m_members.begin(), m_members.end() };
    }

    template<template<typename TDataType> typename TVariableType>
    typename Structure<TVariableType>::ConstVariableContainer Structure<TVariableType>::GetMembers() const
    {
        return { m_members.begin(), m_members.end() };
    }

    template<template<typename TDataType> typename TVariableType>
    template<template<typename OtherDataType> typename OtherVariableType>
    bool Structure<TVariableType>::CheckCompability(const Structure<OtherVariableType>& other) const
    {
        return Private::CheckStructureCompability(
            static_cast<const VariableContainer&>(m_members), 
            static_cast<const typename Structure<OtherVariableType>::VariableContainer&>(other.m_members));
    }

}

namespace Curse::Shader::Private
{

    template<>
    inline bool CheckStructureCompability(
        const Structure<InputVariable>::VariableContainer& lhs, 
        const Structure<OutputVariable>::VariableContainer& rhs)
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }

        for (size_t i = 0; i < lhs.size(); i++)
        {
            auto* memberA = lhs[i];
            auto* memberB = rhs[i];

            if (memberA->GetOutputPin()->GetDataType() != memberB->GetInputPin()->GetDataType())
            {
                return false;
            }
        }

        return true;
    }

    template<>
    inline bool CheckStructureCompability(
        const Structure<OutputVariable>::VariableContainer& lhs,
        const Structure<InputVariable>::VariableContainer& rhs)
    {
        return CheckStructureCompability(rhs, lhs);
    }

}
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

namespace Molten::Shader::Visual
{

    // Structure without any meta data implementations.
    template<template<typename TVariableDataType> typename TVariableType>
    inline Structure<TVariableType, void>::Structure(Script& script) :
        m_script(script),
        m_sizeOf(0)
    {}

    template<template<typename TVariableDataType> typename TVariableType>
    inline Structure<TVariableType, void>::~Structure()
    {
        for (auto& member : m_members)
        {
            delete member;
        }
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableContainer::iterator Structure<TVariableType, void>::begin()
    {
        return m_members.begin();
    }
    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableContainer::const_iterator Structure<TVariableType, void>::begin() const
    {
        return m_members.begin();
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableContainer::iterator Structure<TVariableType, void>::end()
    {
        return m_members.end();
    }
    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableContainer::const_iterator Structure<TVariableType, void>::end() const
    {
        return m_members.end();
    }

    template<template<typename TVariableDataType> typename TVariableType>
    template<typename TDataType>
    inline typename Structure<TVariableType, void>::template VariableType<TDataType>* Structure<TVariableType>::AddMember()
    {
        auto* member = new TVariableType<TDataType>(m_script);
        m_members.push_back(member);
        m_sizeOf += member->GetSizeOf();
        return member;
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline void Structure<TVariableType, void>::RemoveMember(const size_t index)
    {
        if (index >= m_members.size())
        {
            return;
        }
        auto it = m_members.begin() + index;
        auto* variablebase = *it;
        m_sizeOf -= variablebase->GetSizeOf();
        delete variablebase;
        m_members.erase(it);
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline void Structure<TVariableType, void>::RemoveAllMembers()
    {
        for (auto& member : m_members)
        {
            delete member;
        }
        m_members.clear();
        m_sizeOf = 0;
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline size_t Structure<TVariableType, void>::GetMemberCount() const
    {
        return m_members.size();
    }

    template<template<typename TVariableDataType> typename TVariableType>
    template<typename TDataType>
    inline typename Structure<TVariableType, void>::template VariableType<TDataType>* Structure<TVariableType, void>::GetMember(const size_t index)
    {
        return static_cast<VariableType<TDataType>*>(GetMember(index));
    }
    template<template<typename TVariableDataType> typename TVariableType>
    template<typename TDataType>
    inline const typename Structure<TVariableType, void>::template VariableType<TDataType>* Structure<TVariableType, void>::GetMember(const size_t index) const
    {
        return static_cast<const VariableType<TDataType>*>(GetMember(index));
    }
    
    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableBaseType* Structure<TVariableType, void>::GetMember(const size_t index)
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline const typename Structure<TVariableType, void>::VariableBaseType* Structure<TVariableType, void>::GetMember(const size_t index) const
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableBaseType* Structure<TVariableType, void>::operator[](const size_t index)
    {
        return GetMember(index);
    }
    template<template<typename TVariableDataType> typename TVariableType>
    inline const typename Structure<TVariableType, void>::VariableBaseType* Structure<TVariableType, void>::operator[](const size_t index) const
    {
        return GetMember(index);
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::VariableContainer Structure<TVariableType, void>::GetMembers()
    {
        return { m_members.begin(), m_members.end() };
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline typename Structure<TVariableType, void>::ConstVariableContainer Structure<TVariableType, void>::GetMembers() const
    {
        return { m_members.begin(), m_members.end() };
    }

    template<template<typename TVariableDataType> typename TVariableType>
    inline size_t Structure<TVariableType, void>::GetSizeOf() const
    {
        return m_sizeOf;
    }

    template<template<typename TVariableDataType> typename TVariableType>
    template<template<typename TOtherDataType> typename TOtherVariableType>
    inline bool Structure<TVariableType, void>::CheckCompability(const Structure<TOtherVariableType, void>& other) const
    {
        return Private::CheckStructureCompability(
            static_cast<const VariableContainer&>(m_members), 
            static_cast<const typename Structure<TOtherVariableType>::VariableContainer&>(other.m_members));
    }


    // Structure with meta data implementations.
    template<template<typename TVariableDataType> typename TVariableType, typename TMetaData>
    template<typename ... TMetaDataParams>
    Structure<TVariableType, TMetaData>::Structure(Script& script, TMetaDataParams ... metaDataParameters) :
        Structure <TVariableType,void>(script),
        TMetaData(metaDataParameters...)
    { }

}

namespace Molten::Shader::Visual::Private
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
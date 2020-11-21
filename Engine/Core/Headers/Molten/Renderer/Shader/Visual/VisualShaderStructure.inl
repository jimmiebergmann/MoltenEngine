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

#include "Molten/Utility/Template.hpp"
#include <algorithm>

namespace Molten::Shader::Visual
{

    // Structure member base implementations.
    template<typename TMetaData>
    template<typename ... TArgs>
    inline StructureMemberBase<TMetaData>::StructureMemberBase(TArgs ... args) :
        TMetaData(args...)
    {}


    // Structure member implementations.
    template<typename TPinType, typename TMetaData>
    template<typename ... TArgs>
    inline StructureMember<TPinType, TMetaData>::StructureMember(Node& node, TArgs ... args) :
        TPinType(node),
        StructureMemberBase<TMetaData>(args...)
    {}

    template<typename TPinType, typename TMetaData>
    Pin& StructureMember<TPinType, TMetaData>::GetPin()
    {
        return static_cast<Pin&>(*this);
    }

    template<typename TPinType, typename TMetaData>
    const Pin& StructureMember<TPinType, TMetaData>::GetPin() const
    {
        return static_cast<const Pin&>(*this);
    }


    // Structure base implementations.
    template<typename TMetaData>
    inline StructureBase<TMetaData>::StructureBase(Script& script) :
        Node(script)
    {}


    // Structure implementations.
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::Structure(Script& script) :
        StructureBase<TMetaData>(script),
        m_script(script),
        m_members{},
        m_sizeOf(0)
    {}
    
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::~Structure()
    {
        RemoveAllMembers();
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::Iterator
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::begin()
    {
        return m_members.begin();
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::ConstIterator
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::begin() const
    {
        return m_members.begin();
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::Iterator
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::end()
    {
        return m_members.end();
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::ConstIterator
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::end() const
    {
        return m_members.end();
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline NodeType Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetType() const
    {
        return VNodeType;
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline size_t Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return m_members.size();
        }
        else
        {
            return 0;
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline size_t Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return m_members.size();
        }
        else
        {
            return 0;
        }
    }
    
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline Pin* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index >= m_members.size())
            {
                return nullptr;
            }
            return &m_members[index]->GetPin();
        }
        else
        {
            return nullptr;
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline const Pin* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index >= m_members.size())
            {
                return nullptr;
            }
            return &m_members[index]->GetPin();
        }
        else
        {
            return nullptr;
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline InputPin<TDataType>* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return static_cast<InputPin<TDataType>*>(GetInputPin(index));
        }
        else
        {
            return nullptr;
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline const InputPin<TDataType>* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return static_cast<const InputPin<TDataType>*>(GetInputPin(index));
        }
        else
        {
            return nullptr;
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline std::vector<Pin*> Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPins()
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return CreatePinVector<Pin>();
        }
        else
        {
            return {};
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline std::vector<const Pin*> Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetInputPins() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return CreatePinVector<const Pin>();
        }
        else
        {
            return {};
        }
    }
    
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline Pin* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index >= m_members.size())
            {
                return nullptr;
            }
            return &m_members[index]->GetPin();
        }
        else
        {
            return nullptr;
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline const Pin* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index >= m_members.size())
            {
                return nullptr;
            }
            return &m_members[index]->GetPin();
        }
        else
        {
            return nullptr;
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline OutputPin<TDataType>* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return static_cast<OutputPin<TDataType>*>(GetOutputPin(index));
        }
        else
        {
            return nullptr;
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline const OutputPin<TDataType>* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return static_cast<const OutputPin<TDataType>*>(GetOutputPin(index));
        }
        else
        {
            return nullptr;
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline std::vector<Pin*> Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPins()
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return CreatePinVector<Pin>();
        }
        else
        {
            return {};
        }
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline std::vector<const Pin*> Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetOutputPins() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return CreatePinVector<const Pin>();
        }
        else
        {
            return {};
        }
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename TDataType, typename ... TArgs>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::template MemberType<TDataType>&
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::AddMember(TArgs ... args)
    {
        static_assert(TemplateArgumentsContains<TDataType, TAllowedDataTypes...>(),
            "Provided TDataType is not allowed for this structure.");

        auto member = new MemberType<TDataType>(*this, args...);
        m_members.push_back(member);
        m_sizeOf += member->GetSizeOfDataType();
        return *member;
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline void Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::RemoveMember(const size_t index)
    {
        if (index >= m_members.size())
        {
            return;
        }

        auto it = m_members.begin() + index;
        auto* member = *it;
        m_sizeOf -= member->GetPin().GetSizeOfDataType();
        delete member;
        m_members.erase(it);
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline void Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::RemoveMember(const Iterator it)
    {
        auto* member = *it;
        m_sizeOf -= member->GetPin().GetSizeOfDataType();
        delete member;
        m_members.erase(it);
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline void Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::RemoveMember(const ConstIterator it)
    {
        auto* member = *it;
        m_sizeOf -= member->GetPin().GetSizeOfDataType();
        delete member;
        m_members.erase(it);
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline void Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::RemoveAllMembers()
    {
        for (auto* member : m_members)
        {
            delete member;
        }
        m_members.clear();
        m_sizeOf = 0;
    }

    /*template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline MemberBase* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetMember(const size_t index)
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline const MemberBase* Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetMember(const size_t index) const
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }*/

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::MemberBaseType*
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetMemberBase(const size_t index)
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline const typename Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::MemberBaseType*
        Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetMemberBase(const size_t index) const
    {
        if (index >= m_members.size())
        {
            return nullptr;
        }
        return m_members[index];
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline size_t Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetMemberCount() const
    {
        return m_members.size();
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    inline size_t Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::GetSizeOf() const
    {
        return m_sizeOf;
    }


    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<NodeType VOtherNodeType, template<typename> typename TOtherPinType, typename TOtherMetaData, typename ... TOtherAllowedDataTypes>
    bool Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::CompareStructure(
        const Structure<VOtherNodeType, TOtherPinType, TOtherMetaData, TOtherAllowedDataTypes...>& other) const
    {
        if (m_members.size() != other.m_members.size())
        {
            return false;
        }

        for (size_t i = 0; i < m_members.size(); i++)
        {
            if (m_members[i]->GetPin().GetDataType() != other.m_members[i]->GetPin().GetDataType())
            {
                return false;
            }
        }

        return true;
    }

    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    template<typename T>
    inline std::vector<T*> Structure<VNodeType, TPinType, TMetaData, TAllowedDataTypes...>::CreatePinVector() const
    {
        std::vector<T*> pins(m_members.size(), nullptr);
        std::transform(m_members.begin(), m_members.end(), pins.begin(),
            [](auto& member) -> T*
            {
                return &member->GetPin();
            }
        );
        return pins;
    }


    // Meta data constant id implementations.
    template<typename T>
    inline MetaDataConstantId<T>::MetaDataConstantId(const Type id) :
        m_id(id)
    {}

    template<typename T>
    inline typename MetaDataConstantId<T>::Type MetaDataConstantId<T>::GetId() const
    {
        return m_id;
    }

}
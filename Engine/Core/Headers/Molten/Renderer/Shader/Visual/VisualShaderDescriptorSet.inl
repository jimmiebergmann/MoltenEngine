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

namespace Molten::Shader::Visual
{

    // Single descriptor set implementations.
    template<typename ... TAllowedBindingTypes>
    inline DescriptorSet<TAllowedBindingTypes...>::DescriptorSet(Script& script, const uint32_t id) :
        m_script(script),
        m_id(id),
        m_bindings{},
        m_usedBindingIds{}
    {}

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSet<TAllowedBindingTypes...>::Iterator DescriptorSet<TAllowedBindingTypes...>::begin()
    {
        return m_bindings.begin();
    }
    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSet<TAllowedBindingTypes...>::ConstIterator DescriptorSet<TAllowedBindingTypes...>::begin() const
    {
        return m_bindings.begin();
    }

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSet<TAllowedBindingTypes...>::Iterator DescriptorSet<TAllowedBindingTypes...>::end()
    {
        return m_bindings.end();
    }
    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSet<TAllowedBindingTypes...>::ConstIterator DescriptorSet<TAllowedBindingTypes...>::end() const
    {
        return m_bindings.end();
    }

    template<typename ... TAllowedBindingTypes>
    inline Script& DescriptorSet<TAllowedBindingTypes...>::GetScript()
    {
        return m_script;
    }
    template<typename ... TAllowedBindingTypes>
    inline const Script& DescriptorSet<TAllowedBindingTypes...>::GetScript() const
    {
        return m_script;
    }

    template<typename ... TAllowedBindingTypes>
    inline uint32_t DescriptorSet<TAllowedBindingTypes...>::GetId() const
    {
        return m_id;
    }

    template<typename ... TAllowedBindingTypes>
    template<typename TBindingType>
    inline DescriptorBinding<TBindingType>* DescriptorSet<TAllowedBindingTypes...>::AddBinding(const uint32_t id)
    {
       static_assert(TemplateArgumentsContains<DescriptorBinding<TBindingType>, TAllowedBindingTypes...>(), 
            "Provided TBindingType is not allowed for this set.");

        if (m_usedBindingIds.find(id) != m_usedBindingIds.end())
        {
            return nullptr;
        }
 
        auto binding = new DescriptorBinding<TBindingType>(m_script, id);
        m_bindings.push_back(binding);
        m_usedBindingIds.insert(id);

        return binding;
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSet<TAllowedBindingTypes...>::RemoveBinding(const size_t index)
    {
        if (index >= m_bindings.size())
        {
            return;
        }
        auto it = m_bindings.begin() + index;
        m_usedBindingIds.erase((*it)->GetId());
        delete* it;
        m_bindings.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSet<TAllowedBindingTypes...>::RemoveBinding(Iterator it)
    {
        if(it == m_bindings.end())
        {
            MOLTEN_DEBUG_ASSERT(false, "Cannot remove binding of end iterator.");
            return;
        }

        m_usedBindingIds.erase((*it)->GetId());
        delete* it;
        m_bindings.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSet<TAllowedBindingTypes...>::RemoveBinding(ConstIterator it)
    {
        if (it == m_bindings.end())
        {
            MOLTEN_DEBUG_ASSERT(false, "Cannot remove binding of end iterator.");
            return;
        }

        m_usedBindingIds.erase((*it)->GetId());
        delete* it;
        m_bindings.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSet<TAllowedBindingTypes...>::RemoveAllBindings()
    {
        for (auto* binding : m_bindings)
        {
            delete binding;
        }
        m_bindings.clear();
        m_usedBindingIds.clear();
    }

    template<typename ... TAllowedBindingTypes>
    template<typename TBindingType>
    inline DescriptorBinding<TBindingType>* DescriptorSet<TAllowedBindingTypes...>::GetBinding(const size_t index)
    {
        if (index >= m_bindings.size())
        {
            return nullptr;
        }
        return static_cast<DescriptorBinding<TBindingType>*>(m_bindings[index]);
    }
    template<typename ... TAllowedBindingTypes>
    template<typename TBindingType>
    inline const DescriptorBinding<TBindingType>* DescriptorSet<TAllowedBindingTypes...>::GetBinding(const size_t index) const
    {
        if (index >= m_bindings.size())
        {
            return nullptr;
        }
        return static_cast<DescriptorBinding<TBindingType>*>(m_bindings[index]);
    }

    template<typename ... TAllowedBindingTypes>
    inline DescriptorBindingBase* DescriptorSet<TAllowedBindingTypes...>::GetBindingBase(const size_t index)
    {
        if (index >= m_bindings.size())
        {
            return nullptr;
        }
        return m_bindings[index];
    }
    template<typename ... TAllowedBindingTypes>
    inline const DescriptorBindingBase* DescriptorSet<TAllowedBindingTypes...>::GetBindingBase(const size_t index) const
    {
        if (index >= m_bindings.size())
        {
            return nullptr;
        }
        return m_bindings[index];
    }

    template<typename ... TAllowedBindingTypes>
    inline size_t DescriptorSet<TAllowedBindingTypes...>::GetBindingCount() const
    {
        return m_bindings.size();
    }

    template<typename ... TAllowedBindingTypes>
    inline DescriptorSet<TAllowedBindingTypes...>::~DescriptorSet()
    {
        RemoveAllBindings();
    }


    // Multiple descriptor sets implementations.
    template<typename ... TAllowedBindingTypes>
    inline DescriptorSets<TAllowedBindingTypes...>::DescriptorSets(Script& script) :
        m_script(script),
        m_sets{},
        m_usedSetIds{}
    {}

    template<typename ... TAllowedBindingTypes>
    inline DescriptorSets<TAllowedBindingTypes...>::~DescriptorSets()
    {
        RemoveAllSets();
    }

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::Iterator DescriptorSets<TAllowedBindingTypes...>::begin()
    {
        return m_sets.begin();
    }
    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::ConstIterator DescriptorSets<TAllowedBindingTypes...>::begin() const
    {
        return m_sets.begin();
    }

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::Iterator DescriptorSets<TAllowedBindingTypes...>::end()
    {
        return m_sets.end();
    }
    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::ConstIterator DescriptorSets<TAllowedBindingTypes...>::end() const
    {
        return m_sets.end();
    }

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::DescriptorSetType* DescriptorSets<TAllowedBindingTypes...>::AddSet(const uint32_t id)
    {
        if (m_usedSetIds.find(id) != m_usedSetIds.end())
        {
            return nullptr;
        }

        auto* set = new DescriptorSetType(m_script, id);
        m_sets.push_back(set);
        m_usedSetIds.insert(id);

        return set;
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSets<TAllowedBindingTypes...>::RemoveSet(const size_t index)
    {
        if(index >= m_sets.size())
        {
            return;
        }
        auto it = m_sets.begin() + index;
        m_usedSetIds.erase((*it)->GetId());  
        delete* it;
        m_sets.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSets<TAllowedBindingTypes...>::RemoveSet(Iterator it)
    {
        if (it == m_sets.end())
        {
            MOLTEN_DEBUG_ASSERT(false, "Cannot remove set of end iterator.");
            return;
        }

        m_usedSetIds.erase((*it)->GetId());
        delete* it;
        m_sets.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSets<TAllowedBindingTypes...>::RemoveSet(ConstIterator it)
    {
        if (it == m_sets.end())
        {
            MOLTEN_DEBUG_ASSERT(false, "Cannot remove set of end iterator.");
            return;
        }

        m_usedSetIds.erase((*it)->GetId());
        delete* it;
        m_sets.erase(it);
    }

    template<typename ... TAllowedBindingTypes>
    inline void DescriptorSets<TAllowedBindingTypes...>::RemoveAllSets()
    {
        for (auto* set : m_sets)
        {
            delete set;
        }
        m_sets.clear();
        m_usedSetIds.clear();
    }

    template<typename ... TAllowedBindingTypes>
    inline typename DescriptorSets<TAllowedBindingTypes...>::DescriptorSetType* DescriptorSets<TAllowedBindingTypes...>::GetSet(const size_t index)
    {
        if (index >= m_sets.size())
        {
            return nullptr;
        }
        return m_sets[index];
    }
    template<typename ... TAllowedBindingTypes>
    inline const typename DescriptorSets<TAllowedBindingTypes...>::DescriptorSetType* DescriptorSets<TAllowedBindingTypes...>::GetSet(const size_t index) const
    {
        if (index >= m_sets.size())
        {
            return nullptr;
        }
        return m_sets[index];
    }

    template<typename ... TAllowedBindingTypes>
    inline DescriptorSetBase* DescriptorSets<TAllowedBindingTypes...>::GetSetBase(const size_t index)
    {
        if (index >= m_sets.size())
        {
            return nullptr;
        }
        return m_sets[index];
    }
    template<typename ... TAllowedBindingTypes>
    inline const DescriptorSetBase* DescriptorSets<TAllowedBindingTypes...>::GetSetBase(const size_t index) const
    {
        if (index >= m_sets.size())
        {
            return nullptr;
        }
        return m_sets[index];
    }

    template<typename ... TAllowedBindingTypes>
    inline size_t DescriptorSets<TAllowedBindingTypes...>::GetSetCount() const
    {
        return m_sets.size();
    }

}
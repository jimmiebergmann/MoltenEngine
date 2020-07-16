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

#include "Curse/Ecs/EcsEntityTemplate.hpp"

namespace Curse
{

    namespace Ecs
    {

        // Implementations of entity.
        template<typename ContextType>
        inline Entity<ContextType>::Entity() :
            m_metaData(nullptr),
            m_id(-1)
        { }

        template<typename ContextType>
        inline Entity<ContextType>::~Entity()
        { }

        template<typename ContextType>
        inline EntityId Entity<ContextType>::GetEntityId() const
        {
            return m_id;
        }

        template<typename ContextType>
        template<typename ... Components>
        inline void Entity<ContextType>::AddComponents()
        {
            if (m_metaData)
            {
                m_metaData->context->template AddComponents<Components...>(*this);
            }
        }

        template<typename ContextType>
        inline void Entity<ContextType>::RemoveAllComponents()
        {
            if (m_metaData)
            {
                m_metaData->context->RemoveAllComponents(*this);
            }
        }

        template<typename ContextType>
        template<typename ... Components>
        inline void Entity<ContextType>::RemoveComponents()
        {
            if (m_metaData)
            {
                m_metaData->context->template RemoveComponents<Components...>(*this);
            }
        }

        template<typename ContextType>
        template<typename Comp>
        Comp* Entity<ContextType>::GetComponent()
        {
            if (!m_metaData)
            {
                throw Exception("Cannot get component of destroyed entity.");
            }

            return m_metaData->context->template GetComponent<Comp>(*this);
        }
        template<typename ContextType>
        template<typename Comp>
        const Comp* Entity<ContextType>::GetComponent() const
        {
            if (!m_metaData)
            {
                throw Exception("Cannot get component of destroyed entity.");
            }

            return m_metaData->context->template GetComponent<Comp>(*this);
        }

        template<typename ContextType>
        inline void Entity<ContextType>::Destroy()
        {
            if (m_metaData)
            {
                m_metaData->context->DestroyEntity(*this);
            }
        }

        template<typename ContextType>
        inline Entity<ContextType>::Entity(Private::EntityMetaData<ContextType>* metaData, const EntityId id) :
            m_metaData(metaData),
            m_id(id)
        { }


        // Implementations of entity data.
        namespace Private
        {

            template<typename ContextType>
            inline EntityMetaData<ContextType>::EntityMetaData(ContextType* context, const Signature& signature) :
                context(context),
                signature(signature),
                collection(nullptr),
                collectionEntry(0),
                componentGroups{},
                dataPointer(nullptr)
            { }

        }

    }

}
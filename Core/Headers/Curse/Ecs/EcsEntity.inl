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

namespace Curse
{

    namespace Ecs
    {

        // Implementations of entity.
        template<typename ContextType>
        Entity<ContextType>::~Entity()
        { }

        template<typename ContextType>
        inline EntityId Entity<ContextType>::GetEntityId() const
        {
            return m_entityId;
        }

        template<typename ContextType>
        template<typename ... Components>
        inline Entity<ContextType>& Entity<ContextType>::AddComponents()
        {
            if (m_context)
            {
                m_context->AddComponents<Components...>(*this);
            }
            return *this;
        }

        template<typename ContextType>
        inline Entity<ContextType>::Entity(ContextType* context, const EntityId id) :
            m_context(context),
            m_id(id)
        { }


        // Implementations of entity data.
        namespace Private
        {

            template<typename ContextType>
            inline EntityData<ContextType>::EntityData(const Signature& signature, EntityTemplate<ContextType>* entityTemplate) :
                signature(signature),
                entityTemplate(entityTemplate),
                collection(nullptr),
                collectionEntry(0)
            { }

        }

    }

}
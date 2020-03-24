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

#ifndef CURSE_CORE_ECS_ECSENTITY_HPP
#define CURSE_CORE_ECS_ECSENTITY_HPP

#include "Curse/Ecs/Ecs.hpp"
#include "Curse/Ecs/EcsSignature.hpp"

namespace Curse
{

    namespace Ecs
    {

        template<typename Derived> class Context; ///< Forward declaration of Context class.

        using EntityId = int32_t; ///< Data type of entity ID.

        /**
        * @brief Entity object, implicitly containing components.
        */
        template<typename ContextType>
        class Entity
        {

        public:

            /**
            * @brief Virtual destructor.
            */
            virtual ~Entity();

            /**
            * @brief Get id of entity.
            */
            EntityId GetEntityId() const;

            /**
            * @brief Add additional components to entity.
            *        Select components to add via the template parameter list.
            *        Already added components are ignored from the template parameter list.
            */
            template<typename ... Components>
            Entity& AddComponents();

        private:

            /**
            * @brief Private constructor, only called by ContextType.
            */
            Entity(ContextType* context, const EntityId id);

            ContextType* m_context; ///< Pointer to parent context.
            EntityId m_id; ///< Id of this entity.      

            friend typename ContextType; ///< Friend class.

        };



        namespace Private
        {

            template<typename ContextType> class EntityTemplate;
            template<typename ContextType> class EntityTemplateCollection;

            /**
            * @brief Structure of data related to entity, such as information about what collection the entity is part of.
            */
            template<typename ContextType>
            struct EntityData
            {
                EntityData(const Signature& signature, EntityTemplate<ContextType>* entityTemplate);

                Signature signature;
                EntityTemplate<ContextType>* entityTemplate;
                typename EntityTemplateCollection<ContextType>* collection;
                uint16_t collectionIndex;
            };

        }

    }

}

#include "Curse/Ecs/EcsEntity.inl"

#endif
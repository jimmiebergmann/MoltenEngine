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

#ifndef CURSE_CORE_ECS_ECSCONTEXT_HPP
#define CURSE_CORE_ECS_ECSCONTEXT_HPP

#include "Curse/Ecs/Ecs.hpp"
#include "Curse/Ecs/EcsAllocator.hpp"
#include "Curse/Ecs/EcsEntityTemplate.hpp"
#include "Curse/Ecs/EcsSystem.hpp"
#include "Curse/Ecs/EcsEntity.hpp"
#include "Curse/Ecs/EcsComponent.hpp"
#include <map>
#include <queue>
#include <set>
#include <vector>

namespace Curse
{

    namespace Ecs
    {

        /*
        * Descriptor used for constructing a context.
        */
        struct CURSE_API ContextDescriptor
        {
            ContextDescriptor(const size_t memoryBlockSize, const size_t entitiesPerCollection = 20, const size_t reservedComponentsPerGroup = 32);

            size_t memoryBlockSize;
            size_t entitiesPerCollection;
            size_t reservedComponentsPerGroup;
        };


        /*
        * The context in an Entity Component System is the manager of the entire system.
        * It's posible to create multiple, completely isolated from each other.
        * A context is required in order to declare a system or component.
        */
        template<typename DerivedContext>
        class Context
        {

        public:

            /**
            * Register a new system in this context.
            */
            template<typename DerivedSystem, typename ... RequiredComponents>
            void RegisterSystem(System<Context, DerivedSystem, RequiredComponents...>& system);

            /**
            * @brief Create a new entity by providing a set of components to attach to the entity.
            * 
            * All systems who are interested in this entity will be notified.
            * The componenets in interest of the systems will be included in their component groups.
            * The caller is currently responsible of calling DestoryEntity for freeing memory, by calling DestoryEntity.
            */
            template<typename ... Components>
            Entity<Context> CreateEntity();

            /**
            * @brief Destroy an entity.
            *        Memory will be available for other entities.
            */
            void DestroyEntity(Entity<Context>& entity);

            /**
            * @brief Add additional components to entity.
            *        Select components to add via the template parameter list.
            *        Duplicates or already added components are ignored from the template parameter list.
            */
            template<typename ... Components>
            void AddComponents(Entity<Context>& entity);

            /**
            * @brief Remove all components from entity.
            */
            void RemoveAllComponents(Entity<Context>& entity);

            /**
            * @brief Remove components from entity.
            *        Select components to remove via the template parameter list.
            */
            template<typename ... Components>
            void RemoveComponents(Entity<Context>& entity);

            /**
            * @brief Get allocator.
            *        The returned allocated is of type const & by design.
            *        This method makes it possible to access the allocators const functions, making it possible to dump memory if needed.
            */
            const Allocator& GetAlloator() const;

            /**
            * @brief Get entity component.
            *
            * @return Pointer to entity component. Nullptr if provided component is missing in the entity.
            */
            /**@{*/
            template<typename Comp>
            Comp* GetComponent(Entity<Context>& entity);

            template<typename Comp>
            const Comp* GetComponent(const Entity<Context>& entity) const;
            /**@}*/

        protected:

            /**
            * @brief Protected constructor.
            *        User is supposed to create a context class and inherit Context<...>.
            */
            Context(const ContextDescriptor& descriptor);

            /**
            * @brief Destructor.
            *        Destroying all entities, components and deallocating memory.
            */
            ~Context();

        private:

            using Systems = std::set<SystemBase<Context>*>;
            using ComponentGroups = std::map<Signature, Private::ComponentGroup<Context>*>;
            using EntityTemplateMap = std::map<Signature, Private::EntityTemplate<Context>*>;
            using EntityMetaDataMap = std::map<EntityId, Private::EntityMetaData<Context>*>;

            /*
            * @throw Pointer to found entity template, nullptr if no entity template with provided signature exists.
            */
            Private::EntityTemplate<Context>* FindEntityTemplate(const Signature& signature);

            /*
            * @brief Create a new entity template.
            *
            * @throw Exception if entity template with provided signature already existed,
            *        or if provided entitySize is greater than block size in allocator.
            */
            Private::EntityTemplate<Context>* CreateEntityTemplate(const Signature& signature, const size_t entitySize, Private::ComponentOffsetList&& componentOffsets);
   
            /**
            * @brief Get the next available entity ID, destroyed entity ID's are queued for reuse.
            *
            * @return m_freeEntityIds.top() is returned if available, else m_nextEntityId++;
            */
            EntityId GetNextEntityId();

            /**
            * @brief entity id to context for reuse.
            */
            void ReturnEntityId(const EntityId entityId);

            /**
            * @brief Call constructors of provided components, and apply the data to the entity data pointer by the offset list.
            */
            /**@{*/
            template<typename ... Components, typename OffsetContainer>
            void CallComponentConstructors(Byte * entityDataPointer, const OffsetContainer& offsets);

            template<typename ... Components, typename OffsetContainer1, typename OffsetContainer2>
            void CallComponentConstructors(Byte* entityDataPointer, const OffsetContainer1& constructOffsets, const OffsetContainer2& ignoreOffsets);
            /**@}*/

            void InternalRemoveAllComponents(Entity<Context<DerivedContext>>& entity);


            ContextDescriptor m_descriptor;         ///< Context descriptor, containing configurations. 
            Allocator m_allocator;                  ///< Memory allocator, taking care of memory allocations.
            ComponentGroups m_componentGroups;      ///< Container of all component groups.
            EntityTemplateMap m_entityTemplates;    ///< Map of all entity templates.
            EntityMetaDataMap m_entities;           ///< Map of all entities.
            EntityId m_nextEntityId;                ///< The next availalbe entity ID.
            std::queue<EntityId> m_freeEntityIds;   ///< Queue of deleted entity ID's, ready for reuse.
            Systems m_systems;                      ///< Set of registered systems.

        };

    }

}

#include "Curse/Ecs/EcsContext.inl"

#endif
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
#include "Curse/Ecs/EcsSystem.hpp"
#include "Curse/Ecs/EcsEntity.hpp"
#include "Curse/Ecs/EcsComponent.hpp"
#include <map>
#include <queue>

namespace Curse
{

    namespace Ecs
    {

        /*
        * Descriptor used for constructing a context.
        */
        struct CURSE_API ContextDescritor
        {
            ContextDescritor(const size_t memoryBlockSize, const uint16_t entitiesPerCollection = 20, const size_t reservedComponentsPerGroup = 32);

            size_t memoryBlockSize;
            uint16_t entitiesPerCollection;
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
            inline Entity<Context> CreateEntity();

            /**
            * @brief Destroy an entity.
            *        Memory will be available for other entities.
            */
            ///void DestroyEntity(Entity<Context>& entity);

            /**
            * @brief Add one or more components to an already created entity.
            */
            template<typename ... Components>
            Context& AddComponents(const Entity<Context> entity);

            /**
            * @brief Get allocator.
            *        The returned allocated is of type const & by design.
            *        This method makes it possible to access the allocators const functions, making it possible to dump memory if needed.
            */
            const Allocator& GetAlloator() const;

        protected:

            /**
            * @brief Protected constructor.
            *        User is supposed to create a context class and inherit Context<...>.
            */
            Context(const ContextDescritor& descriptor);

            /**
            * @brief Destructor.
            *        Destroying all entities, components and deallocating memory.
            */
            ~Context();

        private:

            /**
            * @brief Function for components to get their component type IDs from.
            */
            static ComponentTypeId GetNextComponentTypeId();

            /**
            * @brief Structure of system and its components signatures
            */
            struct SystemItem
            {
                SystemBase<Context>* system;
                Signature signature;
            };

            /**
            * @return Entity template structure, containing data of all entities of the same component sets.
            */
            struct EntityTemplate
            {

                /**
                * @return Structure of entity template collection data.
                *         A collection contains a set of entities, mapped to memory.
                */
                struct Collection
                {

                    Collection(void* data, const uint16_t entitiesPerCollection, const size_t blockIndex, const size_t dataIndex);

                    /**
                    * @return True if this collection is full, else false.
                    */
                    bool IsFull() const;

                    /**
                    * @return Index of next avilalble entity in this collection.
                    */
                    uint16_t GetFreeEntry();

                    /**
                    * @brief Return an used entity, back to the collection.
                    */
                    void ReturnEntry(const uint16_t entryId);

                    
                    void* data;
                    uint16_t entitiesPerCollection;
                    size_t blockIndex;
                    size_t dataIndex;
                    uint16_t lastFreeEntry;

                    template<typename T>
                    using PriorityQueue = std::priority_queue<T, std::vector<T>, std::greater<uint16_t> >;

                    PriorityQueue<uint16_t> freeEntries;
                };

                EntityTemplate(const size_t entitySize, std::vector<Private::ComponentOffsetItem>&& componentOffsets);
                ~EntityTemplate();
                Collection* GetFreeCollection(Allocator& allocator, const uint16_t entitiesPerCollection);

                using Collections = std::vector<Collection*>;
                Collections collections;
                const size_t entitySize;                                          ///< Total size in bytes of a single entity.
                const std::vector<Private::ComponentOffsetItem> componentOffsets; ///< Offsets of the entities components.
            };

            using Systems = std::vector<SystemItem>;
            using ComponentGroups = std::map<Signature, Private::ComponentGroup<Context>*>;
            using EntityTemplateMap = std::map<Signature, EntityTemplate*>;
            
            ContextDescritor m_descriptor;          ///< Context descriptor, containing configurations. 
            Allocator m_allocator;                  ///< Memory allocator, taking care of memory allocations.
            ComponentGroups m_componentGroups;      ///< Container of all component groups.
            EntityTemplateMap m_entityTemplates;    ///< Map of all entity templates.
            EntityId m_nextEntityId;
            Systems m_systems;

            /**
            * @brief Friend classes.
            */
            /**@{*/ 
            template<typename ContextType, typename DerivedComponent> friend class Component;
            /**@}*/

        };

    }

}

#include "Curse/Ecs/EcsContext.inl"

#endif
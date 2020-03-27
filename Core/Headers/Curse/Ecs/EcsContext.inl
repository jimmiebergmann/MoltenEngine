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

#include <algorithm>
#include <vector>

namespace Curse
{

    namespace Ecs
    {

        /// Implementations of Context.
        template<typename DerivedContext>
        template<typename DerivedSystem, typename ... RequiredComponents>
        inline void Context<DerivedContext>::RegisterSystem(System<Context, DerivedSystem, RequiredComponents...>& system)
        {
            auto* systemPtr = &system;
            for (auto& sys : m_systems)
            {
                if (sys.system == systemPtr)
                {
                    return;
                }
            }

            auto signature = ComponentSignature<RequiredComponents...>::signature;
            m_systems.push_back({ systemPtr, signature });

            auto cgIt = m_componentGroups.find(signature);
            if (cgIt == m_componentGroups.end())
            {
                constexpr size_t componentCount = sizeof...(RequiredComponents);
                size_t componentsReserved = componentCount * m_descriptor.reservedComponentsPerGroup;

                auto* componentGroup = new Private::ComponentGroup<Context>(componentCount);
                componentGroup->systems.reserve(8); // HARDCODED VALUE HERE.
                componentGroup->systems.push_back(systemPtr);
                componentGroup->components.reserve(componentsReserved);

                m_componentGroups.insert({ signature, componentGroup });

                systemPtr->InternalOnRegister(componentGroup);
            }
            else
            {
                auto& systems = cgIt->second->systems;
                bool found = false;
                for (auto* sys : systems)
                {
                    if (sys == systemPtr)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    systems.push_back(systemPtr);
                }
            }
        }

        template<typename DerivedContext>
        template<typename ... Components>
        inline Entity<Context<DerivedContext> > Context<DerivedContext>::CreateEntity()
        {
            constexpr auto entitySize = Private::GetComponenetsSize<Components...>();
            const auto& signature = ComponentSignature<Components...>::signature;

            // Find the indices for each component, sorted by ComponentIds.
            constexpr size_t offsetCount = Private::ComponentOffsets<Components...>::offsetCount;
            const auto offsets = Private::ComponentOffsets<Components...>::offsets;

            // Get entity template, or create a new one if missing,
            Private::EntityTemplate<Context>* entityT = nullptr;
            auto etIt = m_entityTemplates.find(signature);
            if (etIt != m_entityTemplates.end())
            {
                entityT = etIt->second;
            }
            else
            {
                entityT = new Private::EntityTemplate<Context>(m_descriptor.entitiesPerCollection, entitySize, { offsets.begin(), offsets.end() });
                m_entityTemplates.insert({ signature, entityT });
            }

            // Create entity!
            EntityId entityId = GetNextEntityId();

            auto entityData = new Private::EntityData<Context>(signature);
            m_entities.insert({ entityId, entityData });

            Entity<Context> entity(this, entityId);

            if constexpr (entitySize > 0)
            {
                // Get the last collection of this template.
                auto collection = entityT->GetFreeCollection(m_allocator);
                entityData->templateCollection = collection;
                const uint16_t collectionEntry = collection->GetFreeEntry();
                entityData->collectionEntry = collectionEntry;
                const size_t collectionEntryOffset = static_cast<size_t>(collectionEntry) * entitySize;

                Byte* entityDataPtr = collection->GetData() + collectionEntryOffset;
                entityData->data = entityDataPtr;

                /// Call component constructors.
                for (auto& offset : offsets)
                {
                    ForEachTemplateType<Components...>([this, &entityDataPtr, &offset](auto type)
                    {
                        using Type = typename decltype(type)::Type;

                        if (Type::componentTypeId == offset.componentTypeId)
                        {
                            auto componentData = entityDataPtr + offset.offset;
                            Type* component = reinterpret_cast<Type*>(componentData);
                            *component = Type();
                        }
                    });
                }

                // Loop throguh the systems component groups and add the indicies if needed.
                for (auto& pair : m_componentGroups)
                {
                    auto& groupSignature = pair.first;
                    if ((groupSignature & signature) != pair.first)
                    {
                        continue;
                    }

                    auto* componentGroup = pair.second;

                    // Get a vector of component data pointers or relevance for this component group.
                    std::vector<ComponentBase*> componentPointers;
                    for (size_t i = 0; i < offsetCount; i++)
                    {
                        auto& offset = offsets[i];
                        if (groupSignature.IsSet(offset.componentTypeId))
                        {
                            auto componentBase = reinterpret_cast<ComponentBase*>(entityDataPtr + offset.offset);
                            componentPointers.push_back(componentBase);
                        }
                    }

                    // Add component pointers to the component group.
                    if (componentPointers.size())
                    {
                        auto& components = componentGroup->components;
                        auto low = std::lower_bound(components.begin(), components.end(), componentPointers[0]);
                        components.insert(low, componentPointers.begin(), componentPointers.end());
                    }

                    componentGroup->entityCount++;

                    // Notify all systems in interest of this entity signature about entity creation.
                    for (auto* system : componentGroup->systems)
                    {
                        system->InternalOnCreateEntity(entity);
                    }
                }
            }          

            return entity;
        }

        template<typename DerivedContext>
        void Context<DerivedContext>::DestroyEntity(Entity<Context<DerivedContext> >& entity)
        {
            if (!entity.m_context)
            {
                return;
            }
            entity.m_context = nullptr;

            auto entityId = entity.GetEntityId();
            auto eIt = m_entities.find(entityId);
            if (eIt == m_entities.end())
            {
                return;
            }

            auto entityData = eIt->second;
            auto& signature = entityData->signature;
            auto collection = entityData->templateCollection;
            if (collection)
            {
                for (auto& pair : m_componentGroups)
                {
                    if ((pair.first & signature) != pair.first)
                    {
                        continue;
                    }

                    auto* componentGroup = pair.second;
                    auto& components = componentGroup->components;

                    auto low = std::lower_bound(components.begin(), components.end(), entityData->data);
                    if (low == components.end())
                    {
                        continue;
                    }

                    components.erase(low, low + componentGroup->componentsPerEntity);
                    componentGroup->entityCount--;

                    for (auto* system : componentGroup->systems)
                    {
                        system->InternalOnDestroyEntity(entity);
                    }
                }
            }
            
            collection->ReturnEntry(entityData->collectionEntry);
            m_freeEntityIds.push(entityId);

            delete entityData;
            m_entities.erase(eIt);
        }

        template<typename DerivedContext>
        template<typename ... Components>
        void Context<DerivedContext>::AddComponents(const Entity<Context<DerivedContext>> entity)
        {
            constexpr size_t componentAddCount = sizeof...(Components);
            if constexpr (componentAddCount == 0)
            {
                return;
            }

            if (!entity.m_context)
            {
                return;
            }

            constexpr auto entitySize = Private::GetComponenetsSize<Components...>();
            

           

            //const auto& signature = ComponentSignature<Components...>::signature;


        }

        template<typename DerivedContext>
        inline const Allocator& Context<DerivedContext>::GetAlloator() const
        {
            return m_allocator;
        }

        template<typename DerivedContext>
        inline Context<DerivedContext>::Context(const ContextDescritor& descriptor) :
            m_descriptor(descriptor),
            m_allocator(descriptor.memoryBlockSize),
            m_nextEntityId(0)
        {
        }

        template<typename DerivedContext>
        inline Context<DerivedContext>::~Context()
        {
            for (auto pair : m_entities)
            {
                delete pair.second;
            }

            for (auto pair : m_entityTemplates)
            {
                delete pair.second;
            }

            for (auto pair : m_componentGroups)
            {
                delete pair.second;
            }
        }       

        template<typename DerivedContext>
        inline ComponentTypeId Context<DerivedContext>::GetNextComponentTypeId()
        {
            static ComponentTypeId currentComponentTypeId = 0;
            return currentComponentTypeId++;
        }

        template<typename DerivedContext>
        inline EntityId Context<DerivedContext>::GetNextEntityId()
        {
            if (m_freeEntityIds.size())
            {
                auto entityId = m_freeEntityIds.front();
                m_freeEntityIds.pop();
                return entityId;
            }
            return m_nextEntityId++;
        }

    }

}




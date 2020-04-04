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

#include "Curse/Utility/SmartFunction.hpp"
#include <algorithm>
#include <vector>
#include <cstring>
#include <memory>

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
            if (m_systems.find(systemPtr) != m_systems.end())
            {
                return;
            }

            auto signature = ComponentSignature<RequiredComponents...>::signature;
            m_systems.insert({ systemPtr });

            // Create new component group of systems signature if needed.
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
            // The component group already exists, append system to the group.
            else
            {
                auto* componentGroup = cgIt->second;
                componentGroup->systems.push_back(systemPtr);
                systemPtr->InternalOnRegister(componentGroup);
            }
        }

        template<typename DerivedContext>
        template<typename ... Components>
        inline Entity<Context<DerivedContext> > Context<DerivedContext>::CreateEntity()
        {
            static_assert(Private::AreExplicitContextComponentTypes<Context, Components...>(), "Implicit component type.");

            // Make sure the size of each component is larger than 0 bytes.
            ForEachTemplateArgument<Components...>([](auto type)
            {
                using Type = typename decltype(type)::Type;
                static_assert(sizeof(Type) != 0, "Component of size 0 is not supported.");                
            });

            constexpr auto entitySize = Private::GetComponentsSize<Components...>();
            const auto& signature = ComponentSignature<Components...>::signature;

            // Create the entity.
            EntityId entityId = GetNextEntityId();
            auto metaData = std::make_unique< Private::EntityMetaData<Context> >(this, signature);
            
            Entity<Context> entity(metaData.get(), entityId);

            Private::EntityTemplateCollection<Context>* collection = nullptr;
            Private::CollectionEntryId collectionEntry = 0;
            bool gotCollectionEntry = false;

            SmartFunction errorCleaner([&]()
            {
                if (collection && gotCollectionEntry)
                {
                    collection->ReturnEntry(collectionEntry);
                }

                ReturnEntityId(entityId);
            });

            if constexpr (entitySize > 0)
            {
                // Find the data offset of each component, sorted and unsorted by componentTypeId.
                auto& orderedOffsets = Private::OrderedComponentOffsets<Components...>::offsets;
                auto& unorderedOffsets = Private::UnorderedComponentOffsets<Components...>::offsets;

                // Get entity template, or create a new one if missing,
                auto *entityTemplate = FindEntityTemplate(signature);
                if (!entityTemplate)
                {
                    entityTemplate = CreateEntityTemplate(signature, entitySize, { orderedOffsets.begin(), orderedOffsets.end() });
                }

                // Get a new collection and its data.
                collection = entityTemplate->GetFreeCollection(m_allocator);               
                collectionEntry = collection->GetFreeEntry(); 
                gotCollectionEntry = true;
                const size_t collectionEntryOffset = static_cast<size_t>(collectionEntry) * entitySize;
                Byte* dataPointer = collection->GetData() + collectionEntryOffset;

                metaData->collection = collection;
                metaData->collectionEntry = collectionEntry;
                metaData->dataPointer = dataPointer;

                /// Call component constructors.
                ForEachTemplateArgumentIndexed<Components...>([this, &dataPointer, &unorderedOffsets, &entitySize](auto type, const size_t index)
                {
                    using Type = typename decltype(type)::Type;

                    auto* componentData = dataPointer + unorderedOffsets[index].offset;
                    Type* component = reinterpret_cast<Type*>(componentData);
                    *component = Type();
                });

                // Loop throguh the systems component groups and add the indicies if needed.
                for (auto& pair : m_componentGroups)
                {
                    auto& groupSignature = pair.first;
                    if ((groupSignature & signature) != groupSignature)
                    {
                        continue;
                    }

                    auto* componentGroup = pair.second;

                    // Get a vector of component data pointers or relevance for this component group.
                    std::vector<ComponentBase*> componentPointers;
                    for (size_t i = 0; i < orderedOffsets.size(); i++)
                    {
                        auto& offset = orderedOffsets[i];
                        if (groupSignature.IsSet(offset.componentTypeId))
                        {
                            auto componentBase = reinterpret_cast<ComponentBase*>(dataPointer + offset.offset);
                            componentPointers.push_back(componentBase);
                        }
                    }

                    if (!componentPointers.size())
                    {
                        continue;
                    }

                    // Add component pointers to the component group at correct location,
                    // making sure content of component list is linear in memory.
                    auto& components = componentGroup->components;
                    auto low = std::lower_bound(components.begin(), components.end(), componentPointers[0]);
                    components.insert(low, componentPointers.begin(), componentPointers.end());
                    componentGroup->entityCount++;
                    metaData->componentGroups.push_back(componentGroup);

                    // Notify all systems in interest of this entity signature about entity creation.
                    for (auto* system : componentGroup->systems)
                    {
                        system->InternalOnCreateEntity(entity);
                    }
                }
            }          

            errorCleaner.Release();
            m_entities.insert({ entityId,  metaData.release() });

            return entity;
        }

        template<typename DerivedContext>
        void Context<DerivedContext>::DestroyEntity(Entity<Context<DerivedContext> >& entity)
        {
            auto* metaData = entity.m_metaData;
            if (!metaData)
            {
                return;
            }  
            entity.m_metaData = nullptr;

            auto entityId = entity.GetEntityId();
            auto eIt = m_entities.find(entityId);
            if (eIt == m_entities.end())
            {
                delete metaData;
                return;
            }

            auto& componentGroups = metaData->componentGroups;

            for (auto& componentGroup : componentGroups)
            {
                auto& components = componentGroup->components;

                auto low = std::lower_bound(components.begin(), components.end(), static_cast<void*>(metaData->dataPointer));
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
            
            auto collection = metaData->collection;
            if (collection)
            {
                collection->ReturnEntry(metaData->collectionEntry);
            }
            
            ReturnEntityId(entityId);

            delete metaData;
            m_entities.erase(eIt);
        }

        template<typename DerivedContext>
        template<typename ... Components>
        void Context<DerivedContext>::AddComponents(const Entity<Context<DerivedContext>>& entity)
        {
            static_assert(Private::AreExplicitContextComponentTypes<Context, Components...>(), "Implicit component type.");

            // Make sure the size of each component is larger than 0 bytes.
            ForEachTemplateArgument<Components...>([](auto type)
            {
                using Type = typename decltype(type)::Type;
                static_assert(sizeof(Type) != 0, "Component of size 0 is not supported.");
            });

            // Ignore if template parameter list is empty.
            constexpr size_t componentsCount = sizeof...(Components);
            if constexpr (componentsCount == 0)
            {
                return;
            }
            else
            {
                // Make sure the meta data is set, or else the entity is probably destroyed.
                // Also check if entity is part of this context.
                auto* metaData = entity.m_metaData;
                if (!metaData || metaData->context != this)
                {
                    return;
                }

                // Make sure that this component is part of this context.
                auto entityId = entity.GetEntityId();
                auto eIt = m_entities.find(entityId);
                if (eIt == m_entities.end())
                {
                    return;
                }

                // Ignore this call if the new signature is the same  as the old one.
                const auto& componentsSignature = ComponentSignature<Components...>::signature;
                auto& oldSignature = metaData->signature;
                auto newSignature = oldSignature | componentsSignature;
                if (newSignature == oldSignature)
                {
                    return;
                }

                // Get old entity data.
                static const Private::ComponentOffsetList s_emptyOffsetList = {};

                auto* oldOffsets = &s_emptyOffsetList;
                auto* oldCollection = metaData->collection;
                Private::EntityTemplate<Context>* oldEntityTemplate = nullptr;
                size_t oldEntitySize = 0;
                if (oldCollection)
                {
                    oldEntityTemplate = oldCollection->GetEntityTemplate();
                    oldOffsets = &oldEntityTemplate->componentOffsets;
                    oldEntitySize = oldEntityTemplate->entitySize;
                }

                // Get new entity data.
                constexpr auto componentsSize = Private::GetComponentsSize<Components...>();
                auto newEntitySize = oldEntitySize + componentsSize;
                auto newOffsets = *oldOffsets;
                Private::ExtendComponentOffsetList<Components...>(newOffsets, oldEntitySize);

                auto* newEntityTemplate = FindEntityTemplate(newSignature);
                if (!newEntityTemplate)
                {
                    newEntityTemplate = CreateEntityTemplate(newSignature, newEntitySize, Private::ComponentOffsetList(newOffsets));
                }

                // Return old entry to old collection.
                if (oldCollection)
                {
                    oldCollection->ReturnEntry(metaData->collectionEntry);
                }

                if (newEntitySize > 0)
                {
                    auto* newCollection = newEntityTemplate->GetFreeCollection(m_allocator);
                    const auto newCollectionEntry = newCollection->GetFreeEntry();
                    const size_t newCollectionEntryOffset = static_cast<size_t>(newCollectionEntry) * newEntitySize;
                    Byte* newDataPointer = newCollection->GetData() + newCollectionEntryOffset;

                    // We need to move old components and construct new ones.
                    if (oldEntitySize > 0)
                    {
                        auto* oldDataPointer = metaData->dataPointer;

                        std::vector<size_t> constructorOffsets;
                        size_t oldOffsetIndex = 0;
                        for (size_t i = 0; i < newOffsets.size(); i++)
                        {
                            auto& newOffset = newOffsets[i];

                            auto oIt = find_if(oldOffsets->begin(), oldOffsets->end(), [&newOffset](const auto& a) {
                                return a.componentTypeId == newOffset.componentTypeId;
                            });

                            if (oIt == oldOffsets->end())
                            {
                                constructorOffsets.push_back(newOffset.offset);
                            }
                            else
                            {
                                size_t nextNewOffset = (i + 1 < newOffsets.size()) ? newOffsets[i + 1].offset : newEntitySize;

                                size_t copyFromLocation = (*oldOffsets)[oldOffsetIndex].offset;
                                size_t copyToLocation = newOffset.offset;
                                size_t componentSize = nextNewOffset - newOffset.offset;
                                std::memcpy(newDataPointer + copyToLocation, oldDataPointer + copyFromLocation, componentSize);
                                ++oldOffsetIndex;
                            }
                        }

                        ForEachTemplateArgumentIndexed<Components...>([&newDataPointer, &constructorOffsets](auto type, const size_t index)
                        {
                            using Type = typename decltype(type)::Type;

                            auto componentData = newDataPointer + constructorOffsets[index];
                            Type* component = reinterpret_cast<Type*>(componentData);
                            *component = Type();
                        });

                        // Components in old component groups are pointing to the wrong data pointer location now.
                        // Let's remove them and add them later again, pointing to the new data pointer.
                        for (auto& componentGroup : metaData->componentGroups)
                        {
                            auto& components = componentGroup->components;

                            auto low = std::lower_bound(components.begin(), components.end(), static_cast<void*>(metaData->dataPointer));
                            if (low != components.end())
                            {
                                components.erase(low, low + componentGroup->componentsPerEntity);
                            }
                        }
                    }
                    // No old components, let's construct all new ones.
                    else
                    {
                        ForEachTemplateArgumentIndexed<Components...>([this, &newDataPointer, &newOffsets](auto type, const size_t index)
                        {
                            using Type = typename decltype(type)::Type;

                            auto componentData = newDataPointer + newOffsets[index].offset;
                            Type* component = reinterpret_cast<Type*>(componentData);
                            *component = Type();
                        });
                    }

                    for (auto& pair : m_componentGroups)
                    {
                        auto& groupSignature = pair.first;
                        if ((groupSignature & oldSignature) == groupSignature)
                        {
                            continue;
                        }

                        auto* componentGroup = pair.second;
                        auto& components = componentGroup->components;

                        std::vector<ComponentBase*> componentPointers;
                        for (size_t i = 0; i < newOffsets.size(); i++)
                        {
                            auto& offset = newOffsets[i];
                            if (groupSignature.IsSet(offset.componentTypeId))
                            {
                                auto componentBase = reinterpret_cast<ComponentBase*>(newDataPointer + offset.offset);
                                componentPointers.push_back(componentBase);
                            }
                        }

                        if (componentPointers.size() == 0)
                        {
                            continue;
                        }

                        auto low = std::lower_bound(components.begin(), components.end(), componentPointers[0]);
                        components.insert(low, componentPointers.begin(), componentPointers.end());

                        // New component group.
                        // FIX HERE, IGNORE OLD COMPONENT GROUP.
                        if ((groupSignature & newSignature) == groupSignature)
                        {
                            componentGroup->entityCount++;
                            metaData->componentGroups.push_back(componentGroup);

                            // Notify all systems in interest of this entity signature about entity creation.
                            for (auto* system : componentGroup->systems)
                            {
                                system->InternalOnCreateEntity(entity);
                            }
                        }
                    }

                    // Set the new meta data.
                    metaData->signature = newSignature;
                    metaData->collection = newCollection;
                    metaData->collectionEntry = newCollectionEntry;
                    metaData->dataPointer = newDataPointer;
                }
            }
        }

        template<typename DerivedContext>
        inline const Allocator& Context<DerivedContext>::GetAlloator() const
        {
            return m_allocator;
        }

        template<typename DerivedContext>
        template<typename Comp>
        Comp* Context<DerivedContext>::GetComponent(Entity<Context>& entity)
        {
            auto* metaData = entity.m_metaData;
            if (!metaData || !metaData->collection)
            {
                return nullptr;
            }

            auto* entityTemplate = metaData->collection->GetEntityTemplate();

            auto it = entityTemplate->componentOffsetMap.find(Comp::componentTypeId);
            if (it == entityTemplate->componentOffsetMap.end())
            {
                return nullptr;
            }

            size_t offset = it->second;
            return reinterpret_cast<Comp*>(metaData->dataPointer + offset);
        }
        template<typename DerivedContext>
        template<typename Comp>
        const Comp* Context<DerivedContext>::GetComponent(const Entity<Context>& entity) const
        {
            auto* metaData = entity.m_metaData;
            if (!metaData || !metaData->collection)
            {
                return nullptr;
            }

            auto* entityTemplate = metaData->collection->GetEntityTemplate();

            auto it = entityTemplate->componentOffsetMap.find(Comp::componentTypeId);
            if (it == entityTemplate->componentOffsetMap.end())
            {
                return nullptr;
            }

            size_t offset = it->second;
            return reinterpret_cast<Comp*>(metaData->dataPointer + offset);
        }

        template<typename DerivedContext>
        inline Context<DerivedContext>::Context(const ContextDescriptor& descriptor) :
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
        inline Private::EntityTemplate<Context<DerivedContext> >* Context<DerivedContext>::FindEntityTemplate(const Signature& signature)
        {
            auto etIt = m_entityTemplates.find(signature);
            if (etIt != m_entityTemplates.end())
            {
                return etIt->second;
            }
            return nullptr;
        }

        template<typename DerivedContext>
        inline Private::EntityTemplate<Context<DerivedContext> >* Context<DerivedContext>::CreateEntityTemplate(
            const Signature& signature, const size_t entitySize, Private::ComponentOffsetList&& componentOffsets)
        {
           
            size_t maxEntitiesPerCollection = m_allocator.GetBlockSize() / entitySize;
            size_t entitiesPerCollection = std::min(maxEntitiesPerCollection, m_descriptor.entitiesPerCollection);
            if (!entitiesPerCollection)
            {
                throw Exception("Unable to create new entity template(" + std::to_string(entitySize) +
                    " bytes), due to lack of memory to support it. Block size(" +
                    std::to_string(m_allocator.GetBlockSize()) + " bytes) of allocator is too low.");
            }

            auto entityTemplate = new Private::EntityTemplate<Context>(entitiesPerCollection, entitySize, std::move(componentOffsets));
            auto it = m_entityTemplates.insert({ signature, entityTemplate });
            if (!it.second)
            {
                throw Exception("Create new entity template for already existing entity template signature.");
            }

            return entityTemplate;
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

        template<typename DerivedContext>
        void Context<DerivedContext>::ReturnEntityId(const EntityId entityId)
        {
            if (m_nextEntityId && entityId == m_nextEntityId - 1)
            {
                m_nextEntityId--;
                return;
            }
            m_freeEntityIds.push(entityId);
        }

    }

}




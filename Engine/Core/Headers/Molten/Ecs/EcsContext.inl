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

#include "Molten/Utility/SmartFunction.hpp"
#include <algorithm>
#include <vector>
#include <cstring>
#include <memory>

namespace Molten
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

                auto* componentGroup = new Private::ComponentGroup<Context>(signature, componentCount);
                componentGroup->systems.reserve(8); // HARDCODED VALUE HERE.
                componentGroup->systems.push_back(systemPtr);
                componentGroup->components.reserve(componentsReserved);

                m_componentGroups.insert({ signature, componentGroup });

                systemPtr->InternalOnRegister(this, componentGroup);
            }
            // The component group already exists, append system to the group.
            else
            {
                auto* componentGroup = cgIt->second;
                componentGroup->systems.push_back(systemPtr);
                systemPtr->InternalOnRegister(this, componentGroup);
            }
        }

        template<typename DerivedContext>
        template<typename DerivedSystem, typename ... RequiredComponents>
        inline void Context<DerivedContext>::UnregisterSystem(System<Context, DerivedSystem, RequiredComponents...>& system)
        {
            auto* componentGroup = system.m_componentGroup;
            if (componentGroup)
            {
                auto it = std::find(componentGroup->systems.begin(), componentGroup->systems.end(), &system);
                componentGroup->systems.erase(it);
            }

            auto it = m_systems.find(&system);
            if (it != m_systems.end())
            {
                m_systems.erase(it);
            }
            
            system.InternalOnUnregister();
        }

        template<typename DerivedContext>
        template<typename ... Components>
        inline Entity<Context<DerivedContext> > Context<DerivedContext>::CreateEntity()
        {
            static_assert(Private::AreExplicitContextComponentTypes<Context, Components...>(), "Implicit component type.");

            // Make sure the size of each component is larger than 0 bytes.
            ForEachTemplateType<Components...>([](auto type)
            {
                using Type = typename decltype(type)::Type;
                static_assert(sizeof(Type) != 0, "Component of size 0 is not supported.");                
            });

            auto entitySize = Private::ComponentSize<Components...>::uniqueSize;
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

            if(entitySize > 0)
            {
                // Find the data offset of each component, sorted and unsorted by componentTypeId.
                const auto& orderedUniqueOffsets = Private::OrderedComponentOffsets<Components...>::uniqueOffsets;
                const auto& unorderedUniqueOffsets = Private::UnorderedComponentOffsets<Components...>::uniqueOffsets;

                // Get entity template, or create a new one if missing,
                auto *entityTemplate = FindEntityTemplate(signature);
                if (!entityTemplate)
                {
                    entityTemplate = CreateEntityTemplate(signature, entitySize, { orderedUniqueOffsets.begin(), orderedUniqueOffsets.end() });
                }

                // Get a new collection and its data.
                collection = entityTemplate->GetFreeCollection(m_allocator);               
                collectionEntry = collection->GetFreeEntry(); 
                gotCollectionEntry = true;
                const size_t collectionEntryOffset = static_cast<size_t>(collectionEntry) * entitySize;
                Byte* entityDataPointer = collection->GetData() + collectionEntryOffset;

                metaData->collection = collection;
                metaData->collectionEntry = collectionEntry;
                metaData->dataPointer = entityDataPointer;

                /// Call component constructors.
                CallComponentConstructors<Components...>(entityDataPointer, unorderedUniqueOffsets);

                // Loop throguh the systems component groups and add the indicies if needed.
                for (auto& pair : m_componentGroups)
                {
                    auto& groupSignature = pair.first;
                    if ((groupSignature & signature) != groupSignature)
                    {
                        continue;
                    }

                    // Add components to component group.
                    auto* componentGroup = pair.second;
                    componentGroup->AddEntityComponents(entityDataPointer, orderedUniqueOffsets);
                    metaData->componentGroups.push_back(componentGroup);

                    // Notify all systems in interest of this entity signature about entity creation.
                    for (auto* system : componentGroup->systems)
                    {
                        system->InternalOnCreateEntity(entity);
                    }
                }
            }          

            errorCleaner.Release();
            m_entities.insert({ entityId, metaData.release() });

            return entity;
        }

        template<typename DerivedContext>
        inline void Context<DerivedContext>::DestroyEntity(Entity<Context<DerivedContext> >& entity)
        {
            auto* metaData = entity.m_metaData;
            if (!metaData)
            {
                return;
            }  
            entity.m_metaData = nullptr;
            auto entityId = entity.m_id;
            entity.m_id = -1;

            auto eIt = m_entities.find(entityId);
            if (eIt == m_entities.end())
            {
                delete metaData;
                return;
            }

            auto& componentGroups = metaData->componentGroups;
            for (auto& componentGroup : componentGroups)
            {
                componentGroup->EraseEntityComponents(metaData->dataPointer);

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
        inline void Context<DerivedContext>::AddComponents(Entity<Context<DerivedContext>>& entity)
        {
            static_assert(Private::AreExplicitContextComponentTypes<Context, Components...>(), "Implicit component type.");

            // Ignore if template parameter list is empty.
            constexpr size_t componentsCount = sizeof...(Components);
            if constexpr (componentsCount == 0)
            {
                return;
            }
            else
            {
                // Make sure the size of each component is larger than 0 bytes.
                ForEachTemplateType<Components...>([](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    static_assert(sizeof(Type) != 0, "Component of size 0 is not supported.");
                });
          
                // Make sure the meta data is set, or else the entity is probably destroyed.
                // Also check if entity is part of this context.
                auto* metaData = entity.m_metaData;
                if (!metaData || metaData->context != this)
                {
                    return;
                }

                // Make sure that this component is part of this context.
                const auto entityId = entity.GetEntityId();
                if (m_entities.find(entityId) == m_entities.end())
                {
                    return;
                }

                // Ignore this call if the new signature is the same  as the old one.
                const auto& componentsSignature = ComponentSignature<Components...>::signature;
                const auto& oldSignature = metaData->signature;
                const auto newSignature = oldSignature | componentsSignature;
                if (newSignature == oldSignature)
                {
                    return;
                }

                // Get old entity data.
                static const Private::ComponentOffsetList s_emptyOffsetList = {};

                auto* oldOrderedUniqueOffsets = &s_emptyOffsetList;
                auto* oldCollection = metaData->collection;
                Private::EntityTemplate<Context>* oldEntityTemplate = nullptr;
                size_t oldEntitySize = 0;
                if (oldCollection)
                {
                    oldEntityTemplate = oldCollection->GetEntityTemplate();
                    oldOrderedUniqueOffsets = &oldEntityTemplate->componentOffsets;
                    oldEntitySize = oldEntityTemplate->entitySize;
                }

                // Get new entity data.
                auto newOrderedUniqueOffsets = *oldOrderedUniqueOffsets;
                const auto& addingOrderedOffsets = Private::OrderedComponentOffsets<Components...>::uniqueOffsets;
                Private::ExtendOrderedUniqueComponentOffsets(newOrderedUniqueOffsets, addingOrderedOffsets);

                auto componentsSize = Private::ComponentSize<Components...>::uniqueSize;
                auto duplicateComponentSize = Private::GetDuplicateComponentSize(addingOrderedOffsets, *oldOrderedUniqueOffsets);
                auto newEntitySize = oldEntitySize + componentsSize - duplicateComponentSize;

                auto* newEntityTemplate = FindEntityTemplate(newSignature);
                if (!newEntityTemplate)
                {
                    newEntityTemplate = CreateEntityTemplate(newSignature, newEntitySize, Private::ComponentOffsetList(newOrderedUniqueOffsets));
                }

                // Return old entry to old collection.
                if (oldCollection)
                {
                    oldCollection->ReturnEntry(metaData->collectionEntry);
                }

                auto* newCollection = newEntityTemplate->GetFreeCollection(m_allocator);
                const auto newCollectionEntry = newCollection->GetFreeEntry();
                const size_t newCollectionEntryOffset = static_cast<size_t>(newCollectionEntry) * newEntitySize;
                Byte* newEntityDataPointer = newCollection->GetData() + newCollectionEntryOffset;

                if (oldEntitySize > 0)
                {
                    // Copy old data to new data pointer.
                    Private::MigrationComponentOffsetList oldOrderedMigrationOffsets;
                    Private::ComponentOffsetList newUnorderedConstructorOffsets;
                    Private::MigrateAddComponents<Components...>(*oldOrderedUniqueOffsets, newOrderedUniqueOffsets,
                                                                 oldOrderedMigrationOffsets, newUnorderedConstructorOffsets);
                    
                    auto* oldEntityDataPointer = oldCollection->GetData() + (static_cast<size_t>(metaData->collectionEntry) * oldEntitySize);
                    for (auto& offset : oldOrderedMigrationOffsets)
                    {
                        auto* destination = newEntityDataPointer + offset.newOffset;
                        auto* source = oldEntityDataPointer + offset.oldOffset;
                        std::memcpy(destination, source, offset.componentSize);
                    }
                    
                    // Call constructors of new components
                    CallComponentConstructors<Components...>(newEntityDataPointer, newUnorderedConstructorOffsets, *oldOrderedUniqueOffsets);

                    // Update old component groups with new component base pointers.
                    for (auto& pair : m_componentGroups)
                    {
                        auto& groupSignature = pair.first;
                        if ((groupSignature & oldSignature) != groupSignature)
                        {
                            continue;
                        }

                        auto* componentGroup = pair.second;

                        // Update the data pointers in the component group, by removing and adding.
                        componentGroup->EraseEntityComponents(oldEntityDataPointer, *oldOrderedUniqueOffsets);
                        componentGroup->AddEntityComponents(newEntityDataPointer, newOrderedUniqueOffsets);
                    }
                }
                else
                {
                    // No previous components, let's just call the constructors for the new ones.
                    const auto& unorderedUniqueOffsets = Private::UnorderedComponentOffsets<Components...>::uniqueOffsets;
                    CallComponentConstructors<Components...>(newEntityDataPointer, unorderedUniqueOffsets);
                }

                // Add component pointers to new component groups of interest.
                for (auto& pair : m_componentGroups)
                {
                    auto& groupSignature = pair.first;
                    if ((groupSignature & oldSignature) == groupSignature ||
                        (groupSignature & newSignature) != groupSignature)
                    {
                        continue;
                    }

                    // Add components to component group.
                    auto* componentGroup = pair.second;
                    componentGroup->AddEntityComponents(newEntityDataPointer, newOrderedUniqueOffsets);
                    metaData->componentGroups.push_back(componentGroup);

                    // Notify all systems in interest of this entity signature about entity creation.
                    for (auto* system : componentGroup->systems)
                    {
                        system->InternalOnCreateEntity(entity);
                    }
                }

                // Set the new meta data.
                metaData->signature = newSignature;
                metaData->collection = newCollection;
                metaData->collectionEntry = newCollectionEntry;
                metaData->dataPointer = newEntityDataPointer;
            }
        }

        template<typename DerivedContext>
        inline void Context<DerivedContext>::RemoveAllComponents(Entity<Context>& entity)
        {
            // Make sure the meta data is set, or else the entity is probably destroyed.
            // Also check if entity is part of this context.
            auto* metaData = entity.m_metaData;
            if (!metaData || metaData->context != this || !metaData->collection)
            {
                return;
            }

            // Make sure that this component is part of this context.
            const auto entityId = entity.GetEntityId();
            if (m_entities.find(entityId) == m_entities.end())
            {
                return;
            }

            InternalRemoveAllComponents(entity);
        }

        template<typename DerivedContext>
        template<typename ... Components>
        inline void Context<DerivedContext>::RemoveComponents(Entity<Context>& entity)
        {
            static_assert(Private::AreExplicitContextComponentTypes<Context, Components...>(), "Implicit component type.");

            // Ignore if template parameter list is empty.
            constexpr size_t componentsCount = sizeof...(Components);
            if constexpr (componentsCount == 0)
            {
                return;
            }
            else
            {
                // Make sure the size of each component is larger than 0 bytes.
                ForEachTemplateType<Components...>([](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    static_assert(sizeof(Type) != 0, "Component of size 0 is not supported.");
                });

                // Make sure the meta data is set, or else the entity is probably destroyed.
                // Also check if entity is part of this context.
                auto* metaData = entity.m_metaData;
                if (!metaData || metaData->context != this || !metaData->collection)
                {
                    return;
                }

                // Make sure that this component is part of this context.
                const auto entityId = entity.GetEntityId();
                if (m_entities.find(entityId) == m_entities.end())
                {
                    return;
                }

                // Calculate signatures.
                const auto& componentsSignature = ComponentSignature<Components...>::signature;
                const auto& oldSignature = metaData->signature;
                const auto removeSignature = oldSignature & componentsSignature;
                if (!removeSignature.IsAnySet())
                {
                    return;
                }
                const auto newSignature = oldSignature & (~componentsSignature);
                
                if (newSignature.IsAnySet())  // Remove specific components.  
                {
                    // Get old entity data.
                    auto* oldCollection = metaData->collection;
                    auto* oldEntityTemplate = oldCollection->GetEntityTemplate();
                    auto& oldOrderedUniqueOffsets = oldEntityTemplate->componentOffsets;
                    size_t oldEntitySize = oldEntityTemplate->entitySize;
                    
                    // Get new entity data.
                    const auto& removingOrderedUniqueOffsets = Private::OrderedComponentOffsets<Components...>::uniqueOffsets;
                    Private::MigrationComponentOffsetList oldOrderedMigrationOffsets;
                    Private::ComponentOffsetList newOrderedUniqueOffsets;
                    size_t removeComponentsSize = 0;

                    Private::MigrateRemoveComponents(oldOrderedUniqueOffsets, removingOrderedUniqueOffsets,
                                                     oldOrderedMigrationOffsets, newOrderedUniqueOffsets, removeComponentsSize);

                    auto newEntitySize = oldEntitySize - removeComponentsSize;
                    auto* newEntityTemplate = FindEntityTemplate(newSignature);
                    if (!newEntityTemplate)
                    {
                        newEntityTemplate = CreateEntityTemplate(newSignature, newEntitySize, Private::ComponentOffsetList(newOrderedUniqueOffsets));
                    }
                    
                    // Return old entry to old collection.
                    oldCollection->ReturnEntry(metaData->collectionEntry);

                    auto* newCollection = newEntityTemplate->GetFreeCollection(m_allocator);
                    const auto newCollectionEntry = newCollection->GetFreeEntry();
                    const size_t newCollectionEntryOffset = static_cast<size_t>(newCollectionEntry) * newEntitySize;
                    Byte* newEntityDataPointer = newCollection->GetData() + newCollectionEntryOffset;
                    
                    auto* oldEntityDataPointer = oldCollection->GetData() + (static_cast<size_t>(metaData->collectionEntry) * oldEntitySize);
                    for (auto& offset : oldOrderedMigrationOffsets)
                    {
                        auto* destination = newEntityDataPointer + offset.newOffset;
                        auto* source = oldEntityDataPointer + offset.oldOffset;
                        std::memcpy(destination, source, offset.componentSize);
                    }

                    // Set the new meta data.
                    metaData->signature = newSignature;
                    metaData->collection = newCollection;
                    metaData->collectionEntry = newCollectionEntry;
                    metaData->dataPointer = newEntityDataPointer;
                    
                    //for (auto& pair : *componentGroups)
                    auto& componentGroups = metaData->componentGroups;
                    for(auto it = componentGroups.begin(); it != componentGroups.end();)
                    {
                        auto* componentGroup = *it;
                        auto& groupSignature = componentGroup->signature;

                        if ((groupSignature & newSignature) != groupSignature)
                        {
                            it = componentGroups.erase(it);

                            // Remove component pointers from component groups not anymore of interest.
                            componentGroup->EraseEntityComponents(oldEntityDataPointer, oldOrderedUniqueOffsets);
                            
                            for (auto* system : componentGroup->systems)
                            {
                                system->InternalOnDestroyEntity(entity);
                            }
                        }
                        else
                        {
                            ++it;

                            // Update data pointers in component groups still of interest.
                            componentGroup->EraseEntityComponents(oldEntityDataPointer, oldOrderedUniqueOffsets);
                            componentGroup->AddEntityComponents(newEntityDataPointer, newOrderedUniqueOffsets);
                        }
                    }
                }
                else 
                {
                    // Remove all components.
                    InternalRemoveAllComponents(entity);
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
        inline Comp* Context<DerivedContext>::GetComponent(Entity<Context>& entity)
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
        inline const Comp* Context<DerivedContext>::GetComponent(const Entity<Context>& entity) const
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
        inline void Context<DerivedContext>::ReturnEntityId(const EntityId entityId)
        {
            if (m_nextEntityId && entityId == m_nextEntityId - 1)
            {
                m_nextEntityId--;
                return;
            }
            m_freeEntityIds.push(entityId);
        }

        template<typename DerivedContext>
        template<typename ... Components, typename OffsetContainer>
        inline void Context<DerivedContext>::CallComponentConstructors(Byte* entityDataPointer, const OffsetContainer& offsets)
        {
            std::vector<ComponentTypeId> visitedComponents;
            size_t index = 0;

            ForEachTemplateType<Components...>([&](auto type)
            {
                using Type = typename decltype(type)::Type;

                if (std::find(visitedComponents.begin(), visitedComponents.end(), Type::componentTypeId) == visitedComponents.end())
                {
                    visitedComponents.push_back(Type::componentTypeId);

                    Type* component = reinterpret_cast<Type*>(entityDataPointer + offsets[index].offset);
                    *component = Type();

                    index++;
                }
            });
        }

        template<typename DerivedContext>
        template<typename ... Components, typename OffsetContainer1, typename OffsetContainer2>
        inline void Context<DerivedContext>::CallComponentConstructors(Byte* entityDataPointer, const OffsetContainer1& constructOffsets, const OffsetContainer2& ignoreOffsets)
        {
            std::vector<ComponentTypeId> visitedComponents;
            for (auto& offset : ignoreOffsets)
            {
                visitedComponents.push_back(offset.componentTypeId);
            }

            size_t index = 0;

            ForEachTemplateType<Components...>([&](auto type)
            {
                using Type = typename decltype(type)::Type;

                if (std::find(visitedComponents.begin(), visitedComponents.end(), Type::componentTypeId) == visitedComponents.end())
                {
                    visitedComponents.push_back(Type::componentTypeId);

                    Type* component = reinterpret_cast<Type*>(entityDataPointer + constructOffsets[index].offset);
                    *component = Type();

                    index++;
                }
            });
        }

        template<typename DerivedContext>
        inline void Context<DerivedContext>::InternalRemoveAllComponents(Entity<Context<DerivedContext>>& entity)
        {
            auto* metaData = entity.m_metaData;

            auto& componentGroups = metaData->componentGroups;
            for (auto& componentGroup : componentGroups)
            {
                componentGroup->EraseEntityComponents(metaData->dataPointer);

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

            metaData->signature.UnsetAll();
            metaData->collection = nullptr;
            metaData->collectionEntry = 0;
            metaData->componentGroups.clear();
            metaData->dataPointer = nullptr;
        }
       

    }

}




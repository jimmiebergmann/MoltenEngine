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
            EntityId entityId = m_nextEntityId++;
            //auto entityData = new EntityData(signature, entityT);
            //m_entities.push_back(entityData);

            Entity<Context> entity(this, entityId);

            if constexpr (entitySize > 0)
            {
                // Get the last collection of this template.
                auto collection = entityT->GetFreeCollection(m_allocator);
                const size_t collectionBlockIndex = collection->GetBlockIndex();
                const size_t collectionDataIndex = collection->GetDataIndex();
                const uint16_t collectionEntry = collection->GetFreeEntry();
                const size_t collectionEntryOffset = static_cast<size_t>(collectionEntry) * entitySize;

                /// Call constructors.
                for (auto& offset : offsets)
                {
                    ForEachTemplateType<Components...>([this, &offset, &collection, &collectionEntryOffset](auto type)
                    {
                        using Type = typename decltype(type)::Type;

                        if (Type::componentTypeId == offset.componentTypeId)
                        {
                            auto componentData = reinterpret_cast<uint8_t*>(collection->GetData()) + collectionEntryOffset + offset.offset;
                            Type* component = reinterpret_cast<Type*>(componentData);
                            *component = Type();
                        }
                    });
                }

                // Loop throguh the systems component groups and add the indicies if needed.
                for (auto& pair : m_componentGroups)
                {
                    if ((pair.first & signature) == pair.first)
                    {
                        auto* componentGroup = pair.second;

                        for (size_t i = 0; i < offsetCount; i++)
                        {
                            auto& offset = offsets[i];
                            if (pair.first.IsSet(offset.componentTypeId))
                            {
                                auto dataOffset = collectionDataIndex + collectionEntryOffset + offset.offset;
                                auto dataPointer = reinterpret_cast<uint8_t*>(m_allocator.GetBlock(collectionBlockIndex)) + dataOffset;
                                componentGroup->components.push_back(reinterpret_cast<ComponentBase*>(dataPointer));
                            }
                        }

                        componentGroup->entityCount++;

                        for (auto* system : componentGroup->systems)
                        {
                            system->InternalOnCreateEntity(entity);
                        }
                    }
                }
            }          

            return entity;
        }

        template<typename DerivedContext>
        template<typename ... Components>
        Context<DerivedContext>& Context<DerivedContext>::AddComponents(const Entity<Context> entity)
        {
            if (!entity.m_context)
            {
                return *this;
            }

            return *this;
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

    }

}




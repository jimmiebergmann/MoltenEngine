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
            constexpr auto componentsSize = Private::GetComponenetsSize<Components...>();
            const auto& signature = ComponentSignature<Components...>::signature;

            // Find the indices for each component, sorted by ComponentIds.
            constexpr size_t offsetCount = Private::ComponentOffsets<Components...>::offsetCount;
            const auto offsets = Private::ComponentOffsets<Components...>::offsets;

            // Get entity template, or create a new one if missing,
            EntityTemplate* entityT = nullptr;
            auto etIt = m_entityTemplates.find(signature);
            if (etIt != m_entityTemplates.end())
            {
                entityT = etIt->second;
            }
            else
            {
                entityT = new EntityTemplate(componentsSize, { offsets.begin(), offsets.end() });
                m_entityTemplates.insert({ signature, entityT });
            }

            // Create entity!
            EntityId entityId = m_nextEntityId++;
            //auto entityData = new EntityData(signature, entityT);
            //m_entities.push_back(entityData);

            Entity<Context> entity(this, entityId);

            if constexpr (componentsSize > 0)
            {
                // Get the last collection of this template.
                /*auto collection = */entityT->GetFreeCollection(m_allocator, m_descriptor.entitiesPerCollection);
            }          

            return entity;
        }

        template<typename DerivedContext>
        template<typename ... Components>
        Context<DerivedContext>& Context<DerivedContext>::AddComponents(const Entity<Context> /*entity*/)
        {
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

        /// Implementations of entity template collections.
        template<typename DerivedContext>
        inline Context<DerivedContext>::EntityTemplate::Collection::Collection(void* data, const uint16_t entitiesPerCollection, const size_t blockIndex, const size_t dataIndex) :
            data(data),
            entitiesPerCollection(entitiesPerCollection),
            blockIndex(blockIndex),
            dataIndex(dataIndex),
            lastFreeEntry(0)
        { }

        template<typename DerivedContext>
        inline bool Context<DerivedContext>::EntityTemplate::Collection::IsFull() const
        {
            return lastFreeEntry + 1 == entitiesPerCollection;
        }

        template<typename DerivedContext>
        inline uint16_t Context<DerivedContext>::EntityTemplate::Collection::GetFreeEntry()
        {
            if (freeEntries.size())
            {
                auto entry = freeEntries.top();
                freeEntries.pop();
                return entry;
            }

            return lastFreeEntry++;
        }

        template<typename DerivedContext>
        inline void Context<DerivedContext>::EntityTemplate::Collection::ReturnEntry(const uint16_t entryId)
        {
            if (lastFreeEntry && entryId == lastFreeEntry - 1)
            {
                lastFreeEntry--;
                return;
            }
            freeEntries.push(entryId);
        }

        /// Implementations of entity template.
        template<typename DerivedContext>
        inline Context<DerivedContext>::EntityTemplate::EntityTemplate(const size_t entitySize, std::vector<Private::ComponentOffsetItem>&& componentOffsets) :
            entitySize(entitySize),
            componentOffsets(std::move(componentOffsets))
        { }

        template<typename DerivedContext>
        inline Context<DerivedContext>::EntityTemplate::~EntityTemplate()
        {
            /*for (auto* collection : collections)
            {
                delete collection;
            }*/
        }

        template<typename DerivedContext>
        inline Context<DerivedContext>::EntityTemplate::template Collection* Context<DerivedContext>::EntityTemplate::GetFreeCollection(Allocator& allocator, const uint16_t entitiesPerCollection)
        {
            if (!collections.size() || collections.back()->IsFull())
            {
                size_t blockIndex = 0;
                size_t dataIndex = 0;           
                void* data = allocator.RequestMemory(entitySize * static_cast<size_t>(entitiesPerCollection), blockIndex, dataIndex);

                auto collection = new Collection(data, entitiesPerCollection, blockIndex, dataIndex);
                collections.push_back(collection);
                return collection;
            }

            return collections.back();
        }

    }

}




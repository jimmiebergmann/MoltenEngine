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
#include <limits>

namespace Curse
{

    namespace Ecs
    {

        namespace Private
        {

            /// Implementations of entity template collections.
            template<typename ContextType>
            inline EntityTemplateCollection<ContextType>::EntityTemplateCollection(EntityTemplate<ContextType>* entityTemplate, Byte* data,
                                                                                   const size_t blockIndex, const size_t dataIndex, const size_t entitiesPerCollection) :
                entitiesPerCollection(entitiesPerCollection),
                m_entityTemplate(entityTemplate),
                m_data(data),
                m_blockIndex(blockIndex),
                m_dataIndex(dataIndex),
                m_lastFreeEntry(0),
                m_freeEntries{}
            { }

            template<typename ContextType>
            inline size_t EntityTemplateCollection<ContextType>::GetBlockIndex() const
            {
                return m_blockIndex;
            }

            template<typename ContextType>
            inline size_t EntityTemplateCollection<ContextType>::GetDataIndex() const
            {
                return m_dataIndex;
            }

            template<typename ContextType>
            Byte* EntityTemplateCollection<ContextType>::GetData()
            {
                return m_data;
            }
            template<typename ContextType>
            const Byte* EntityTemplateCollection<ContextType>::GetData() const
            {
                return m_data;
            }

            template<typename ContextType>
            EntityTemplate<ContextType>* EntityTemplateCollection<ContextType>::GetEntityTemplate()
            {
                return m_entityTemplate;
            }
            template<typename ContextType>
            const EntityTemplate<ContextType>* EntityTemplateCollection<ContextType>::GetEntityTemplate() const
            {
                return m_entityTemplate;
            }

            template<typename ContextType>
            inline CollectionEntryId EntityTemplateCollection<ContextType>::GetFreeEntry()
            {
                if (m_freeEntries.size())
                {
                    auto entry = m_freeEntries.top();
                    m_freeEntries.pop();
                    return entry;
                }

                return m_lastFreeEntry++;
            }

            template<typename ContextType>
            inline bool EntityTemplateCollection<ContextType>::IsFull() const
            {
                return static_cast<size_t>(m_lastFreeEntry) == entitiesPerCollection && !m_freeEntries.size();
            }

            template<typename ContextType>
            inline void EntityTemplateCollection<ContextType>::ReturnEntry(const CollectionEntryId entryId)
            {
                // SHOULD WE CLEAR? REDUNDANT DUE TO CONSTUCTOR CALL AT COMPONENT CREATION?
                //auto dataPtr = m_data + static_cast<size_t>(entryId) * m_entityTemplate->entitySize;
                //std::memset(dataPtr, 0, m_entityTemplate->entitySize);

                if (m_lastFreeEntry && entryId == m_lastFreeEntry - 1)
                {
                    m_lastFreeEntry--;
                    return;
                }
                m_freeEntries.push(entryId);
            }


            /// Implementations of entity template.
            template<typename ContextType>
            inline EntityTemplate<ContextType>::EntityTemplate(const size_t entitiesPerCollection, const size_t entitySize, Private::ComponentOffsetList&& componentOffsets) :
                entitiesPerCollection(std::min(entitiesPerCollection, static_cast<size_t>(std::numeric_limits<CollectionEntryId>::max() - 1))),
                entitySize(entitySize),
                componentOffsets(std::move(componentOffsets)),
                componentOffsetMap(CreateComponentOffsetMap()),
                collections{}
            { }

            template<typename ContextType>
            inline EntityTemplate<ContextType>::~EntityTemplate()
            {
                for (auto* collection : collections)
                {
                    delete collection;
                }
            }

            template<typename ContextType>
            inline EntityTemplateCollection<ContextType>* EntityTemplate<ContextType>::GetFreeCollection(Allocator& allocator)
            {
                if (!collections.size() || collections.back()->IsFull())
                {
                    size_t blockIndex = 0;
                    size_t dataIndex = 0;
                    Byte* data = allocator.RequestMemory(entitySize * static_cast<size_t>(entitiesPerCollection), blockIndex, dataIndex);

                    auto collection = new EntityTemplateCollection<ContextType>(this, data, blockIndex, dataIndex, entitiesPerCollection);
                    collections.push_back(collection);
                    return collection;
                }

                return collections.back();
            }

            template<typename ContextType>
            inline std::map<ComponentTypeId, size_t> EntityTemplate<ContextType>::CreateComponentOffsetMap()
            {
                std::map<ComponentTypeId, size_t> offsets;

                for (auto& offset : componentOffsets)
                {
                    offsets.insert({offset.componentTypeId, offset.offset});
                }

                return std::move(offsets);
            }

        }

    }

}

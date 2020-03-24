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

        namespace Private
        {

            /// Implementations of entity template collections.
            template<typename ContextType>
            inline EntityTemplateCollection<ContextType>::EntityTemplateCollection(void* data, const size_t blockIndex, const size_t dataIndex, const uint16_t entitiesPerCollection) :
                entitiesPerCollection(entitiesPerCollection),
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
            void* EntityTemplateCollection<ContextType>::GetData()
            {
                return m_data;
            }
            template<typename ContextType>
            const void* EntityTemplateCollection<ContextType>::GetData() const
            {
                return m_data;
            }

            template<typename ContextType>
            inline uint16_t EntityTemplateCollection<ContextType>::GetFreeEntry()
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
                return m_lastFreeEntry + 1 == entitiesPerCollection;
            }

            template<typename ContextType>
            inline void EntityTemplateCollection<ContextType>::ReturnEntry(const uint16_t entryId)
            {
                if (m_lastFreeEntry && entryId == m_lastFreeEntry - 1)
                {
                    lastFreeEntry--;
                    return;
                }
                m_freeEntries.push(entryId);
            }


            /// Implementations of entity template.
            template<typename ContextType>
            inline EntityTemplate<ContextType>::EntityTemplate(const uint16_t entitiesPerCollection, const size_t entitySize, Private::ComponentOffsetItems&& componentOffsets) :
                entitiesPerCollection(entitiesPerCollection),
                entitySize(entitySize),
                componentOffsets(std::move(componentOffsets)),
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
                    void* data = allocator.RequestMemory(entitySize * static_cast<size_t>(entitiesPerCollection), blockIndex, dataIndex);

                    auto collection = new EntityTemplateCollection<ContextType>(data, blockIndex, dataIndex, entitiesPerCollection);
                    collections.push_back(collection);
                    return collection;
                }

                return collections.back();
            }

        }

    }

}

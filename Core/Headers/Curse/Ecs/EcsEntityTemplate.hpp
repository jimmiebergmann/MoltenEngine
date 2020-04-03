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

#ifndef CURSE_CORE_ECS_ECSENTITYTEMPLATE_HPP
#define CURSE_CORE_ECS_ECSENTITYTEMPLATE_HPP

#include "Curse/Ecs/Ecs.hpp"
#include "Curse/Ecs/EcsComponent.hpp"
#include <queue>
#include <vector>
#include <map>

namespace Curse
{

    namespace Ecs
    {

        namespace Private
        {

            // Forward declarations.
            template<typename ContextType> class EntityTemplate;


            using CollectionEntryId = uint8_t; /// < Data type of entity template collection entry ID.


            /**
            * @brief Structure of entity template collection data.
            *         A collection contains a set of entities, mapped to memory.
            */
            template<typename ContextType>
            class EntityTemplateCollection
            {

            public:

                /**
                * @brief Constructor.
                *        Collection is initialized by providing a partial block of data.
                *        
                * @param entitiesPerCollection is clamped to max value of CollectionEntryId.
                */
                EntityTemplateCollection(EntityTemplate<ContextType>* entityTemplate, Byte* data, const size_t blockIndex,
                                         const size_t dataIndex, const size_t entitiesPerCollection);

                /**
                * @return Index of which block the data of this collection is located.
                */
                size_t GetBlockIndex() const;

                /**
                * @return Index of data start, offset from the data pointer passed at construction.
                */
                size_t GetDataIndex() const;

                /**
                * @return Pointer to data start of this collection.
                */
                /**@{*/
                Byte* GetData();
                const Byte* GetData() const;
                /**@}*/

                /**
                * @return Pointer to entity template of this collection.
                */
                /**@{*/
                EntityTemplate<ContextType>* GetEntityTemplate();
                const EntityTemplate<ContextType>* GetEntityTemplate() const;
                /**@}*/

                /**
                * @return Index of next avilalble entity in this collection.
                */
                CollectionEntryId GetFreeEntry();

                /**
                * @return True if this collection is full, else false.
                */
                bool IsFull() const;

                /**
                * @brief Return an used entity, back to the collection.
                */
                void ReturnEntry(const CollectionEntryId entryId);

                const size_t entitiesPerCollection;   ///< Maximum number of enteties of this collection.

            private:

                template<typename T>
                using PriorityQueue = std::priority_queue<T, std::vector<T>, std::greater<CollectionEntryId> >;

                EntityTemplate<ContextType>* m_entityTemplate;      ///< Pointer to parent entity template.
                Byte* m_data;                                       ///< Pointer to data start of this collection.
                size_t m_blockIndex;                                ///< Index of allocator block.
                size_t m_dataIndex;                                 ///< Index of data, of allocator block.
                CollectionEntryId m_lastFreeEntry;                  ///< Current available entry id.
                PriorityQueue<CollectionEntryId> m_freeEntries;     ///< List of free enty id's.

            };


            /**
            * @brief Entity template structure, containing data of all entities of the same component sets.
            */
            template<typename ContextType>
            class EntityTemplate
            {

            public:

                /**
                * @brief Constructor.
                *         Entity templates are constructed, by providing the size in bytes of each entity, and an vector of component offsets.
                */
                EntityTemplate(const size_t entitiesPerCollection, const size_t entitySize, Private::ComponentOffsetList&& componentOffsets);

                /**
                * @brief Destructor. Cleaning up allocated collections.
                */
                ~EntityTemplate();

                /**
                * @brief Get next free collection.
                */
                EntityTemplateCollection<ContextType>* GetFreeCollection(Allocator& allocator);

                const size_t entitiesPerCollection;                         ///< Maximum number of enteties per collection.
                const size_t entitySize;                                    ///< Total size in bytes of a single entity.
                const Private::ComponentOffsetList componentOffsets;        ///< Compoent offsets of this entities components.
                const std::map<ComponentTypeId, size_t> componentOffsetMap; ///< Map of component offsets for this entity template.

            private:

                std::map<ComponentTypeId, size_t> CreateComponentOffsetMap();

                using Collections = std::vector<EntityTemplateCollection<ContextType>*>;

                Collections collections;    ///< Vector of all collections of this template.

            };

        }

    }

}

#include "Curse/Ecs/EcsEntityTemplate.inl"

#endif

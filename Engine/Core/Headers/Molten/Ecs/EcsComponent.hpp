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

#ifndef MOLTEN_CORE_ECS_ECSCOMPONENT_HPP
#define MOLTEN_CORE_ECS_ECSCOMPONENT_HPP

#include "Molten/Ecs/EcsSignature.hpp"
#include <array>
#include <vector>

namespace Molten
{

    namespace Ecs
    {

        using ComponentTypeId = int16_t; ///< Data type of component type ID.


        // Forward declarations.
        namespace Private
        {
            template<typename ContextType> ComponentTypeId GetNextComponentTypeId();
        }
        

        /**
        * @brief Component base class.
        */
        class ComponentBase
        {

        };

        /**
        * @brief Component base class for specific context type.
        */
        template<typename ContextType>
        class ComponentContextBase : public ComponentBase
        {

        };

        /**
        * @brief Component class.
        *        Inherit from this class to create component structures.
        *        Components are implicitly attached to entities.
        */
        template<typename ContextType, typename DerivedComponent>
        class Component : public ComponentContextBase<ContextType>
        {

        public:

            /**
            * @brief Id of this component type.
            */
            static inline const ComponentTypeId componentTypeId = Private::GetNextComponentTypeId<ContextType>();

        };


        template<typename ContextType> class SystemBase; ///< Forward declaration.
     
        namespace Private
        {

            /**
             * @brief Function for components to get their component type IDs from.
             */
            template<typename ContextType>
            ComponentTypeId GetNextComponentTypeId();

            /**
            * @brief Checks if provided types are explicit component types.
            * @return True if Types inherits from ComponentBase, else false.
            */
            template<typename ... Types>
            constexpr bool AreExplicitComponentTypes();

            /**
            * @brief Checks if provided types are explicit component types.
            * @return True if Types inherits from ComponentContextBase<ContextType>, else false.
            */
            template<typename ContextType, typename ... Types>
            constexpr bool AreExplicitContextComponentTypes();


            /**
            * @brief Helper function, count the total number of bytes of all passes components.
            */
            template<typename ... Components>
            size_t GetUniqueComponentSize();

            /**
            * @brief Helper structure, for retreiving total size of multiple components.
            */
            template<typename ... Components>
            struct ComponentSize
            {
                static inline const size_t uniqueSize = GetUniqueComponentSize<Components...>();
            };


            /**
            * @brief Calculate summed size of duplicated offsets.
            *        Provided offset containers must be ordered ny ComponentTypeId and unique.
            */
            template<typename OffsetContainer>
            size_t GetDuplicateComponentSize(const OffsetContainer& firstOffsets, const OffsetContainer& secondOffsets);


            /**
            * @brief Helper structure, containing offset of a component type id.
            *
            * @see OrderedComponentOffsets
            * @see UnorderedComponentOffsets
            */
            struct ComponentOffsetItem
            {
                ComponentTypeId componentTypeId;
                size_t componentSize;
                size_t offset;
            };

            using ComponentOffsetList = std::vector<ComponentOffsetItem>; ///< Vector of component offset items.

            template<size_t Size>
            using ComponentOffsetArray = std::array<ComponentOffsetItem, Size>; ///< Array of component offset items.


            /**
            * @brief Helper structure for migrating components from one offset to another.
            */
            struct MigrationComponentOffsetItem
            {
                size_t componentSize;
                size_t oldOffset;
                size_t newOffset;
            };

            using MigrationComponentOffsetList = std::vector<MigrationComponentOffsetItem>; ///< Vector of migration component offset items.

            /**
            * @brief Helper function for getting a list of migration offsets for component adding, from one component group to another.
            *        The provided offset containers must be ordered.
            */
            template<typename ... Components, typename OffsetContainer1, typename OffsetContainer2>
            void MigrateAddComponents(const OffsetContainer1& oldOrderedUniqueOffsets, const OffsetContainer2& newOrderedUniqueOffsets,
                                      MigrationComponentOffsetList& oldOrderedMigrationComponentOffsets, ComponentOffsetList& newUnorderedConstructorOffsets);

            /**
            * @brief Helper function for getting a list of migration offsets for component removal, from one component group to another.
            *        The provided offset containers must be ordered.
            */
            template<typename OffsetContainer1, typename OffsetContainer2>
            void MigrateRemoveComponents(const OffsetContainer1& oldOrderedUniqueOffsets, const OffsetContainer2& removingOrderedUniqueOffsets,
                                         MigrationComponentOffsetList& oldOrderedMigrationOffsets, ComponentOffsetList& newOrderedUniqueOffsets, size_t& removeComponentsSize);

            /**
            * @brief Structure of components grouped together for systems.
            */
            template<typename ContextType>
            struct ComponentGroup
            {
                /**
                * @brief Constructor of component group.
                */
                ComponentGroup(const Signature& signature, const size_t componentsPerEntity);

                /**
                * @brief Add components to this component group,
                *        by providing a data pointer to the entity and a container of offset items.
                *        The offset items are used for determining what components are of interest.
                */
                template<typename OffsetContainer>
                void AddEntityComponents(Byte* entityDataPointer, const OffsetContainer& offsets);

                /**
                * @brief Erase components from this component group,
                *        by providing a data pointer to the entity.
                */
                /**@{*/
                void EraseEntityComponents(const Byte* entityDataPointer);

                /*
                * @brief The offset items are used for determining what components are of interest.
                */
                template<typename OffsetContainer>
                void EraseEntityComponents(const Byte* entityDataPointer, const OffsetContainer& offsets);
                /**@}*/

                const Signature signature;                      ///< Signature of this component group.
                const size_t componentsPerEntity;               ///< Number of components per entity.
                std::vector<SystemBase<ContextType>*> systems;  ///< Vector of systems interested in this component group.    
                std::vector<ComponentBase*> components;         ///< Vector of all components. The entity stride is defined by componentsPerEntity.
                size_t entityCount;                             ///< Number of entities in this component group.
            };


            /**
            * @brief Helper function, for expanding an array of ordered component offsets.
            */
            template<typename OffsetContainer>
            void ExtendOrderedUniqueComponentOffsets(ComponentOffsetList& offsetList, const OffsetContainer& extendingOffsets);


            /**
            * @brief Helper function, for creating an array of component offsets.
            *        Ordered by componentTypeId of Components.
            */
            template<typename ... Components>
            ComponentOffsetArray<sizeof...(Components)> CreateOrderedComponentOffsets();

            /**
            * @brief Helper function, for creating an array of unique component offsets.
            *        Ordered by componentTypeId of Components.
            *        Duplicates of componentTypeIds are removed and offsets are corrected.
            */
            template<typename ... Components>
            ComponentOffsetList CreateOrderedUniqueComponentOffsets();

            /**
            * @brief Helper function, for creating an array of component offsets.
            *        Ordered by the order of passed Components.
            *        Offset of duplicates of components are set to std::numeric_limit<size_t>::max().
            */
            template<typename ... Components>
            ComponentOffsetArray<sizeof...(Components)> CreateUnorderedComponentOffsets();

            /**
            * @brief Helper function, for creating an array of unique component offsets.
            *        Ordered by componentTypeId of Components.
            *        Duplicates of componentTypeIds are removed and offsets are corrected.
            */
            template<typename ... Components>
            ComponentOffsetList CreateUnorderedUniqueComponentOffsets();

            /**
            * @brief Helper structure, for retreiving data offset of each component.
            *        Ordered by componentTypeId for each component.
            */
            template<typename ... Components>
            struct OrderedComponentOffsets
            {
                static inline const ComponentOffsetArray<sizeof...(Components)> offsets = CreateOrderedComponentOffsets<Components...>();
                static inline const ComponentOffsetList uniqueOffsets = CreateOrderedUniqueComponentOffsets<Components...>();
            };

            /**
            * @brief Helper structure, for retreiving data offset of each component.
            *        Ordered by the order of passed Components.
            *        Offset of duplicates of components are set to std::numeric_limit<size_t>::max().
            */
            template<typename ... Components>
            struct UnorderedComponentOffsets
            {
                static inline const ComponentOffsetArray<sizeof...(Components)> offsets = CreateUnorderedComponentOffsets<Components...>();
                static inline const ComponentOffsetList uniqueOffsets = CreateUnorderedUniqueComponentOffsets<Components...>();
            };


            /**
            * @brief Return the index of provided Comp, in the template parameter set of Components.
            */
            template<typename Comp, typename ... Components>
            size_t GetComponentIndexOfTypes();

            /**
            * @brief Helper structure, for retreiving the offset of a component in a template parameter set.
            */
            template<typename Comp, typename ... Components>
            struct ComponentIndex
            {
                static inline const size_t index = GetComponentIndexOfTypes<Comp, Components...>();
            };

        }

    }

}

#include "Molten/Ecs/EcsComponent.inl"

#endif
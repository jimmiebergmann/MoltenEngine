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

#ifndef CURSE_CORE_ECS_ECSCOMPONENT_HPP
#define CURSE_CORE_ECS_ECSCOMPONENT_HPP

#include "Curse/Ecs/Ecs.hpp"
#include <array>
/*#include "Curse/System/Bitfield.hpp"
#include <vector>
#include <array>
#include <algorithm>
#include <limits>*/

namespace Curse
{

    namespace Ecs
    {

        template<typename ContextType> class SystemBase; ///< Forward declaration.

        using ComponentTypeId = int16_t; ///< Data type of component type ID.

        /*
        
    template<typename ContextType>
    class SystemBase;


    using ComponentTypeIdType = int16_t;


    struct ComponentOffsetItem
    {
        template<typename Comp>
        static void Insert(std::vector<ComponentOffsetItem>& componentOffsets, const size_t fullSize)
        {
            const ComponentTypeIdType componentTypeId = Comp::ComponentTypeId;

            ComponentOffsetItem newItem = { componentTypeId, 0 };

            auto lower = std::lower_bound(componentOffsets.begin(), componentOffsets.end(), newItem,
                [](const auto& a, const auto& b)
            {
                return a.componentTypeId < b.componentTypeId;
            });

            if (lower == componentOffsets.end())
            {
                newItem.offset = fullSize;
                componentOffsets.push_back(newItem);
            }
            else
            {
                newItem.offset = lower->offset;
                auto newIt = componentOffsets.insert(lower, newItem);
                for (auto it = ++newIt; it != componentOffsets.end(); it++)
                {
                    it->offset += sizeof(Comp);
                }
            }

        }

        ComponentTypeIdType componentTypeId;
        size_t offset;
    };

    template<typename ... Components>
    constexpr std::array<ComponentOffsetItem, sizeof...(Components)> CreateComponentOffsets();

    template<typename Comp, typename ... OfComps>
    size_t GetComponentIndexOfTypes();
    */


    /**
    * @brief Component base class.
    */
    class ComponentBase
    {

    };


    /**
    * @brief Component class.
    *        Inherit from this class to create component structures.
    *        Components are implicitly attached to entities.
    */
    template<typename ContextType, typename DerivedComponent>
    class Component : public ComponentBase
    {

    public:

        /**
        * @brief Id of this component type.
        */
        static inline const ComponentTypeId componentTypeId = ContextType::GetNextComponentTypeId();

    };


    namespace Private
    {

        /**
        * @brief Structure of components grouped together for systems.
        */
        template<typename ContextType>
        struct ComponentGroup
        {
            /**
            * @brief Constructor of component group.
            */
            ComponentGroup(const size_t componentsPerEntity);

            std::vector<SystemBase<ContextType>*> systems;  ///< Vector of systems interested in this component group.
            const size_t componentsPerEntity;               ///<  Number of components per entity.
            std::vector<ComponentBase*> components;         ///< Vector of all components. The entity stride is defined by componentsPerEntity.
            size_t entityCount;                             ///< Number of entities in this component group.

            ///std::vector<Entity<ContextType>*> entities;
        };


        /**
        * @brief Count the total number of bytes of all passes components.
        */
        template<typename ... Components>
        constexpr size_t GetComponenetsSize();


        /**
        * @brief Helper structure, containing offset of a component type id.
        * @see ComponentOffsets
        */
        struct ComponentOffsetItem
        {
            ComponentTypeId componentTypeId;
            size_t offset;
        };


        /**
        * @brief Helper function, for creating an array of component offsets.
        */
        template<typename ... Components>
        constexpr std::array<ComponentOffsetItem, sizeof...(Components)> CreateComponentOffsets();
        

        /**
        * @brief Helper structure, for retreiving data offset of each component.
        */
        template<typename ... Components>
        struct ComponentOffsets
        {
            constexpr static size_t offsetCount = sizeof...(Components);
            static inline const std::array<ComponentOffsetItem, sizeof...(Components)> offsets = CreateComponentOffsets<Components...>();
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

    //template<typename ContextType, typename DerivedComponent>
    //const ComponentTypeIdType Component<ContextType, DerivedComponent>::ComponentTypeId = ContextType::GetNextComponentTypeId();
    /*

    using ComponentSignature = Bitmask<CURSE_MAX_COMPONENT_TYPES>;

    template<typename ContextType>
    struct ComponentIndexGroup
    {
        ComponentIndexGroup(const size_t componentsPerEntity) :
            componentsPerEntity(componentsPerEntity),
            entityCount(0)
        { }

        std::vector<SystemBase<ContextType>*> systems;
        const size_t componentsPerEntity;
        std::vector<ComponentBase*> components;

        std::vector<Entity<ContextType>*> entities;

        size_t entityCount;
    };


    template<typename ... Components>
    ComponentSignature CreateComponentSignature()
    {
        ComponentSignature signature;

        LoopTemplateTypes<Components...>([&signature](auto type)
        {
            using Type = typename decltype(type)::Type;
            signature.Set(Type::ComponentTypeId);
        });

        return std::move(signature);
    }


    template<typename ... Components>
    struct ComponentOffsets
    {
        constexpr static size_t OffsetCount = sizeof...(Components);
        static inline const std::array<ComponentOffsetItem, sizeof...(Components)> Offsets = CreateComponentOffsets<Components...>();
    };



    //template<typename ... Components>
    //const std::array<ComponentOffsetItem, sizeof...(Components)> ComponentOffsets<Components...>::offsets = Private::CreateComponentOffsets<Components...>();



    template<typename ... Components>
    constexpr std::array<ComponentOffsetItem, sizeof...(Components)> CreateComponentOffsets()
    {
        struct Item
        {
            ComponentTypeIdType componentTypeId;
            size_t size;

            bool operator <(const Item& item) const
            {
                return componentTypeId < item.componentTypeId;
            }
        };

        std::vector<Item> items;
        LoopTemplateTypes<Components...>([&items](auto type)
        {
            using Type = typename decltype(type)::Type;

            items.push_back({ Type::ComponentTypeId, sizeof(Type) });
        });

        std::sort(items.begin(), items.end());

        std::array<ComponentOffsetItem, sizeof...(Components)> offsets = {};
        size_t sum = 0;
        for (size_t i = 0; i < items.size(); i++)
        {
            offsets[i].offset = sum;
            offsets[i].componentTypeId = items[i].componentTypeId;
            sum += items[i].size;
        }

        return offsets;
    }

    template<typename Comp, typename ... OfComps>
    size_t GetComponentIndexOfTypes()
    {
        // TODO: static_assert with some cool meta programming, like TemplatePackContains<Comp, OfComps...>

        std::vector<ComponentTypeIdType> componentIds;

        LoopTemplateTypes<OfComps...>([&componentIds](auto type)
        {
            using Type = typename decltype(type)::Type;
            componentIds.push_back(Type::ComponentTypeId);
        });

        std::sort(componentIds.begin(), componentIds.end());

        for (size_t i = 0; i < componentIds.size(); i++)
        {
            if (componentIds[i] == Comp::ComponentTypeId)
            {
                return i;
            }
        }

        throw std::runtime_error("Comp is not contained by OfComps");
    }
        
        */


    }

}

#include "Curse/Ecs/EcsComponent.inl"

#endif
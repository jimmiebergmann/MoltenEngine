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


#include "Curse/Utility/Template.hpp"
#include <type_traits>
#include <map>
#include <algorithm>

namespace Curse
{

    namespace Ecs
    {

        namespace Private
        {

            template<typename ContextType>
            inline ComponentTypeId GetNextComponentTypeId()
            {
                static ComponentTypeId currentComponentTypeId = 0;
                return currentComponentTypeId++;
            }

            template<typename ... Types>
            inline constexpr bool AreExplicitComponentTypes()
            {
                return (std::is_base_of<ComponentBase, Types>::value && ...) &&
                       (!std::is_same<ComponentBase, Types>::value && ...);
            }

            template<typename ContextType, typename ... Types>
            inline constexpr bool AreExplicitContextComponentTypes()
            {
                return (std::is_base_of<ComponentContextBase<ContextType>, Types>::value && ...) &&
                       (!std::is_same<ComponentContextBase<ContextType>, Types>::value && ...);
            }

            template<typename ... Components>
            inline constexpr size_t GetComponentsSize()
            {
                static_assert(Private::AreExplicitComponentTypes<Components...>(), "Implicit component type.");

                size_t size = 0;
                ForEachTemplateArgument<Components...>([&size](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    size += sizeof(Type);
                });

                return size;
            }

            template<typename ... Components, typename OffsetContainer>
            inline void MigrateAddComponents(const OffsetContainer& oldOffsetList, const OffsetContainer& newOffsetList,
                                             MigrationComponentOffsetList& oldComponentOffsets, ComponentOffsetList& newComponentOffsets)
            {
                size_t newOffsetIndex = 0;

                for (size_t i = 0; i < oldOffsetList.size(); i++)
                {
                    auto& oldOffset = oldOffsetList[i];
                    for (size_t j = newOffsetIndex; j < newOffsetList.size(); j++)
                    {
                        ++newOffsetIndex;

                        auto& newOffset = newOffsetList[j];
                        if (oldOffset.componentTypeId == newOffset.componentTypeId)
                        {
                            oldComponentOffsets.push_back({ newOffset.componentSize, oldOffset.offset, newOffset.offset });
                            break;
                        }
                    }
                }

                ForEachTemplateArgument<Components...>([&](auto type)
                {
                    using Type = typename decltype(type)::Type;

                    for (auto& offset : newOffsetList)
                    {
                        if (Type::componentTypeId == offset.componentTypeId)
                        {
                            newComponentOffsets.push_back({ offset.componentTypeId, offset.componentSize, offset.offset });
                            break;
                        }
                    }
                });
            }

            template<typename ContextType>
            inline ComponentGroup<ContextType>::ComponentGroup(const Signature& signature, const size_t componentsPerEntity) :
                signature(signature),
                componentsPerEntity(componentsPerEntity),
                entityCount(0)
            { }

            template<typename ContextType>
            template<typename OffsetContainer>
            inline void ComponentGroup<ContextType>::AddEntityComponents(Byte* entityDataPointer, const OffsetContainer& offsets)
            {
                std::vector<ComponentBase*> componentOfInterest;
                for (size_t i = 0; i < offsets.size(); i++)
                {
                    if (signature.IsSet(offsets[i].componentTypeId))
                    {
                        auto* componentBase = reinterpret_cast<ComponentBase*>(entityDataPointer + offsets[i].offset);
                        componentOfInterest.push_back(componentBase);
                    }
                }

                if (componentOfInterest.size())
                {
                    auto low = std::lower_bound(components.begin(), components.end(), componentOfInterest[0]);
                    components.insert(low, componentOfInterest.begin(), componentOfInterest.end());
                    ++entityCount;
                }
            }

            template<typename ContextType>
            inline void ComponentGroup<ContextType>::EraseEntityComponents(const Byte* entityDataPointer)
            {
                auto* firstComponent = reinterpret_cast<const ComponentBase*>(entityDataPointer);
                auto low = std::lower_bound(components.begin(), components.end(), firstComponent);
                if (low != components.end())
                {
                    components.erase(low, low + componentsPerEntity);
                    --entityCount;
                }           
            }

            template<typename ContextType>
            template<typename OffsetContainer>
            inline void ComponentGroup<ContextType>::EraseEntityComponents(const Byte* entityDataPointer, const OffsetContainer& offsets)
            {
                int32_t firstIndex = -1;
                for (int32_t i = 0; i < offsets.size(); i++)
                {
                    if (signature.IsSet(offsets[i].componentTypeId))
                    {
                        firstIndex = i;
                        break;
                    }
                }
                if (firstIndex != -1)
                {
                    auto* firstComponent = reinterpret_cast<const ComponentBase*>(entityDataPointer + offsets[firstIndex].offset);
                    auto low = std::lower_bound(components.begin(), components.end(), firstComponent);
                    components.erase(low, low + componentsPerEntity);
                    --entityCount;
                }
            }


            template<typename ... Components>
            inline void ExtendOrderedComponentOffsetList(ComponentOffsetList& offsetList, const size_t oldTotalSize)
            {
                size_t currentSize = oldTotalSize;

                offsetList.reserve(offsetList.size() + sizeof...(Components));
                ForEachTemplateArgument<Components...>([&offsetList, &currentSize](auto type)
                {
                    using Type = typename decltype(type)::Type;

                    auto lower = std::lower_bound(offsetList.begin(), offsetList.end(), Type::componentTypeId,
                        [](const auto& a, const auto& b)
                    {
                        return a.componentTypeId < b;
                    });

                    if (lower == offsetList.end())
                    {
                        offsetList.push_back({ Type::componentTypeId, sizeof(Type), currentSize });
                    }
                    else
                    {
                        auto newIt = offsetList.insert(lower, { Type::componentTypeId, sizeof(Type), lower->offset });
                        for (auto it = ++newIt; it != offsetList.end(); it++)
                        {
                            it->offset += sizeof(Type);
                        }
                    }

                    currentSize += sizeof(Type);
                });
            }

            template<typename ... Components>
            inline constexpr ComponentOffsetArray<sizeof...(Components)> CreateOrderedComponentOffsets()
            {
                if constexpr (sizeof...(Components) == 0)
                {
                    return {};
                }
                else
                {
                    struct Item
                    {
                        ComponentTypeId componentTypeId;
                        size_t componentSize;

                        bool operator <(const Item& item) const
                        {
                            return componentTypeId < item.componentTypeId;
                        }
                    };

                    std::vector<Item> items;
                    ForEachTemplateArgument<Components...>([&items](auto type)
                    {
                        using Type = typename decltype(type)::Type;
                        items.push_back({ Type::componentTypeId, sizeof(Type) });
                    });

                    std::sort(items.begin(), items.end());

                    ComponentOffsetArray<sizeof...(Components)> offsets = {};
                    size_t sum = 0;
                    for (size_t i = 0; i < items.size(); i++)
                    {
                        offsets[i].componentTypeId = items[i].componentTypeId;
                        offsets[i].componentSize = items[i].componentSize;
                        offsets[i].offset = sum;
                        
                        sum += items[i].componentSize;
                    }

                    return std::move(offsets);
                }
            }

            template<typename ... Components>
            inline constexpr ComponentOffsetArray<sizeof...(Components)> CreateUnorderedComponentOffsets()
            {
                if constexpr (sizeof...(Components) == 0)
                {
                    return {};
                }
                else
                {
                    struct Item
                    {
                        ComponentTypeId componentTypeId;
                        size_t componentSize;

                        bool operator <(const Item& item) const
                        {
                            return componentTypeId < item.componentTypeId;
                        }
                    };

                    std::vector<Item> items;
                    ForEachTemplateArgument<Components...>([&items](auto type)
                    {
                        using Type = typename decltype(type)::Type;
                        items.push_back({ Type::componentTypeId, sizeof(Type) });
                    });

                    std::sort(items.begin(), items.end());

                    std::map<ComponentTypeId, size_t> offsetMap;
                    size_t sum = 0;
                    for (size_t i = 0; i < items.size(); i++)
                    {
                        offsetMap.insert({ items[i].componentTypeId, sum });
                        sum += items[i].componentSize;
                    }

                    ComponentOffsetArray<sizeof...(Components)> offsets = {};
                    ForEachTemplateArgumentIndexed<Components...>([&offsetMap, &offsets](auto type, const size_t index)
                    {
                        using Type = typename decltype(type)::Type;
                        auto it = offsetMap.find(Type::componentTypeId);

                        auto& offset = offsets[index];
                        offset.componentTypeId = Type::componentTypeId;
                        offset.componentSize = sizeof(Type);
                        offset.offset = it->second;
                    });

                    return std::move(offsets);
                }
            }

          
            template<typename Comp, typename ... Components>
            inline size_t GetComponentIndexOfTypes()
            {
                static_assert(TemplateArgumentsContains<Comp, Components...>(),
                    "Provided component is missing in the component template argument list.");
                static_assert(Private::AreExplicitComponentTypes<Components...>(), "Implicit component type.");

                std::vector<ComponentTypeId> componentIds;

                ForEachTemplateArgument<Components...>([&componentIds](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    componentIds.push_back(Type::componentTypeId);
                });

                std::sort(componentIds.begin(), componentIds.end());

                for (size_t i = 0; i < componentIds.size(); i++)
                {
                    if (componentIds[i] == Comp::componentTypeId)
                    {
                        return i;
                    }
                }

                return 0;
            }

        }

    }

}
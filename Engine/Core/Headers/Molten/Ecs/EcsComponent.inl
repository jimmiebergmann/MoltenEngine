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


#include "Molten/Utility/Template.hpp"
#include <type_traits>
#include <map>
#include <algorithm>

namespace Molten
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
            inline size_t GetUniqueComponentSize()
            {
                size_t size = 0;          
                std::vector<ComponentTypeId> visitedOffsets;
                ForEachTemplateArgument<Components...>([&](auto type)
                {
                    using Type = typename decltype(type)::Type;

                    if (std::find(visitedOffsets.begin(), visitedOffsets.end(), Type::componentTypeId) == visitedOffsets.end())
                    {
                        visitedOffsets.push_back(Type::componentTypeId);
                        size += sizeof(Type);
                    }
                });

                return size;
            }

            template<typename OffsetContainer>
            inline size_t GetDuplicateComponentSize(const OffsetContainer& firstOffsets, const OffsetContainer& secondOffsets)
            {
                size_t size = 0;

                auto* first = &firstOffsets;
                auto* second = &secondOffsets;

                if (firstOffsets.size() > secondOffsets.size())
                {
                    std::swap(first, second);
                }

                for (auto& offset1 : *first)
                {
                    for (auto& offset2 : *second)
                    {
                        if (offset1.componentTypeId == offset2.componentTypeId)
                        {
                            size += offset2.componentSize;
                            break;
                        }
                    }
                }

                return size;
            }

            template<typename ... Components, typename OffsetContainer1, typename OffsetContainer2>
            inline void MigrateAddComponents(const OffsetContainer1& oldOrderedUniqueOffsets, const OffsetContainer2& newOrderedUniqueOffsets, 
                                             MigrationComponentOffsetList& oldOrderedMigrationComponentOffsets, ComponentOffsetList& newUnorderedConstructorOffsets)
            {
                std::vector<ComponentTypeId> visitedComponents;

                for (size_t i = 0; i < oldOrderedUniqueOffsets.size(); i++)
                {
                    auto& oldOffset = oldOrderedUniqueOffsets[i];
                    for (size_t j = 0; j < newOrderedUniqueOffsets.size(); j++)
                    {
                        auto& newOffset = newOrderedUniqueOffsets[j];
                        if (oldOffset.componentTypeId == newOffset.componentTypeId)
                        {
                            oldOrderedMigrationComponentOffsets.push_back({ newOffset.componentSize, oldOffset.offset, newOffset.offset });
                            visitedComponents.push_back(oldOffset.componentTypeId);
                            break;
                        }
                    }
                }

                ForEachTemplateArgument<Components...>([&](auto type)
                {
                    using Type = typename decltype(type)::Type;

                    if (std::find(visitedComponents.begin(), visitedComponents.end(), Type::componentTypeId) != visitedComponents.end())
                    {
                        return;
                    }
                    visitedComponents.push_back(Type::componentTypeId);

                    auto it = std::find_if(newOrderedUniqueOffsets.begin(), newOrderedUniqueOffsets.end(), [&](auto a)
                    {
                        return a.componentTypeId == Type::componentTypeId;
                    });
                    
                    newUnorderedConstructorOffsets.push_back(*it);
                });

            }

            template<typename OffsetContainer1, typename OffsetContainer2>
            inline void MigrateRemoveComponents(const OffsetContainer1& oldOrderedUniqueOffsets, const OffsetContainer2& removingOrderedUniqueOffsets,
                MigrationComponentOffsetList& oldOrderedMigrationOffsets, ComponentOffsetList& newOrderedUniqueOffsets, size_t& removeComponentsSize)
            {
                size_t currentOffset = 0;
                for (size_t i = 0; i < oldOrderedUniqueOffsets.size(); i++)
                {
                    bool remove = false;
                    auto& oldOffset = oldOrderedUniqueOffsets[i];
                    for (size_t j = 0; j < removingOrderedUniqueOffsets.size(); j++)
                    {
                        auto& removeOffset = removingOrderedUniqueOffsets[j];

                        if (oldOffset.componentTypeId == removeOffset.componentTypeId)
                        {
                            remove = true;
                            removeComponentsSize += removeOffset.componentSize;
                            break;
                        }
                    }
                    if (!remove)
                    {
                        oldOrderedMigrationOffsets.push_back({ oldOffset.componentSize, oldOffset.offset, currentOffset });
                        newOrderedUniqueOffsets.push_back({ oldOffset.componentTypeId, oldOffset.componentSize, currentOffset });
                        currentOffset += oldOffset.componentSize;
                    }
                }
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
                for (int32_t i = 0; i < static_cast<int32_t>(offsets.size()); i++)
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


            template<typename OffsetContainer>
            inline void ExtendOrderedUniqueComponentOffsets(ComponentOffsetList& offsetList, const OffsetContainer& extendingOffsets)
            {
                size_t currentSize = offsetList.size() ? (offsetList.back().offset + offsetList.back().componentSize) : 0;

                std::vector<ComponentTypeId> visitedComponents;
                for (auto& offset : offsetList)
                {
                    visitedComponents.push_back(offset.componentTypeId);
                }

                for (auto& offset : extendingOffsets)
                {
                    if (std::find(visitedComponents.begin(), visitedComponents.end(), offset.componentTypeId) != visitedComponents.end())
                    {
                        continue;
                    }

                    visitedComponents.push_back(offset.componentTypeId);

                    auto lower = std::lower_bound(offsetList.begin(), offsetList.end(), offset.componentTypeId,
                        [](const auto& a, const auto b)
                    {
                        return a.componentTypeId < b;
                    });

                    if (lower == offsetList.end())
                    {
                        offsetList.push_back({ offset.componentTypeId, offset.componentSize, currentSize });
                    }
                    else
                    {
                        auto newIt = offsetList.insert(lower, { offset.componentTypeId, offset.componentSize, lower->offset });
                        for (auto it = ++newIt; it != offsetList.end(); it++)
                        {
                            it->offset += offset.componentSize;
                        }
                    }

                    currentSize += offset.componentSize;
                }
            }

            template<typename ... Components>
            inline ComponentOffsetArray<sizeof...(Components)> CreateOrderedComponentOffsets()
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
            inline ComponentOffsetList CreateOrderedUniqueComponentOffsets()
            {
                ComponentOffsetList uniqueOffsets;
                auto orderedOffsets = CreateOrderedComponentOffsets<Components...>();
                
                std::vector<ComponentTypeId> visitedOffsets;
                size_t currentOffset = 0;
                for (auto& offset : orderedOffsets)
                {
                    if (std::find(visitedOffsets.begin(), visitedOffsets.end(), offset.componentTypeId) == visitedOffsets.end())
                    {
                        visitedOffsets.push_back(offset.componentTypeId);
                        uniqueOffsets.push_back({ offset.componentTypeId, offset.componentSize, currentOffset });
                        currentOffset += offset.componentSize;
                    }
                }

                return std::move(uniqueOffsets);
            }

            template<typename ... Components>
            inline ComponentOffsetArray<sizeof...(Components)> CreateUnorderedComponentOffsets()
            {
                // Note: Giving incorrect offsets of duplicates.

                if constexpr (sizeof...(Components) == 0)
                {
                    return {};
                }
                else
                {
                    struct ItemSize
                    {
                        ComponentTypeId componentTypeId;
                        size_t componentSize;

                        bool operator <(const ItemSize& item) const
                        {
                            return componentTypeId < item.componentTypeId;
                        }
                    };

                    struct ItemOffset
                    {
                        ComponentTypeId componentTypeId;
                        size_t offset;
                    };

                    std::vector<ItemSize> sizeTypes;
                    ForEachTemplateArgument<Components...>([&sizeTypes](auto type)
                    {
                        using Type = typename decltype(type)::Type;
                        sizeTypes.push_back({ Type::componentTypeId, sizeof(Type) });
                    });
                    std::sort(sizeTypes.begin(), sizeTypes.end());

                    std::vector<ItemOffset> offsetType;
                    size_t offsetSum = 0;
                    for (size_t i = 0; i < sizeTypes.size(); i++)
                    {
                        offsetType.push_back({ sizeTypes[i].componentTypeId, offsetSum });
                        offsetSum += sizeTypes[i].componentSize;
                    }

                    ComponentOffsetArray<sizeof...(Components)> offsets = {};
                    ForEachTemplateArgumentIndexed<Components...>([&offsetType, &offsets](auto type, const size_t index)
                    {
                        using Type = typename decltype(type)::Type;
  
                        for (size_t i = 0; i < offsetType.size(); i++)
                        {
                            if (offsetType[i].componentTypeId == Type::componentTypeId)
                            {
                                auto& offset = offsets[index];
                                offset.componentTypeId = Type::componentTypeId;
                                offset.componentSize = sizeof(Type);
                                offset.offset = offsetType[i].offset;
                                break;
                            }
                        }
                    });

                    return std::move(offsets);
                }
            }

            template<typename ... Components>
            inline ComponentOffsetList CreateUnorderedUniqueComponentOffsets()
            {
                if constexpr (sizeof...(Components) == 0)
                {
                    return {};
                }
                else
                {
                    struct ItemSize
                    {
                        ComponentTypeId componentTypeId;
                        size_t componentSize;

                        bool operator <(const ItemSize& item) const
                        {
                            return componentTypeId < item.componentTypeId;
                        }
                    };

                    struct ItemOffset
                    {
                        ComponentTypeId componentTypeId;
                        size_t offset;
                    };

                    std::vector<ItemSize> sizeTypes;
                    std::vector<ComponentTypeId> visitedOffsets;
                    ForEachTemplateArgument<Components...>([&sizeTypes, &visitedOffsets](auto type)
                    {
                        using Type = typename decltype(type)::Type;

                        if (std::find(visitedOffsets.begin(), visitedOffsets.end(), Type::componentTypeId) == visitedOffsets.end())
                        {
                            visitedOffsets.push_back(Type::componentTypeId);
                            sizeTypes.push_back({ Type::componentTypeId, sizeof(Type) });
                        }
                    });
                    std::sort(sizeTypes.begin(), sizeTypes.end());

                    std::vector<ItemOffset> offsetType;
                    size_t offsetSum = 0;
                    for (size_t i = 0; i < sizeTypes.size(); i++)
                    {
                        offsetType.push_back({ sizeTypes[i].componentTypeId, offsetSum });
                        offsetSum += sizeTypes[i].componentSize;
                    }

                    ComponentOffsetList uniqueOffsets;
                    visitedOffsets.clear();
                    ForEachTemplateArgument<Components...>([&offsetType, &uniqueOffsets, &visitedOffsets](auto type)
                    {
                        using Type = typename decltype(type)::Type;

                        for (size_t i = 0; i < offsetType.size(); i++)
                        {
                            if (offsetType[i].componentTypeId == Type::componentTypeId)
                            {
                                if (std::find(visitedOffsets.begin(), visitedOffsets.end(), Type::componentTypeId) == visitedOffsets.end())
                                {
                                    visitedOffsets.push_back(Type::componentTypeId);
                                    uniqueOffsets.push_back({ Type::componentTypeId, sizeof(Type), offsetType[i].offset }); 
                                }
                                break;
                            }
                        }
                    });

                    return std::move(uniqueOffsets);
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
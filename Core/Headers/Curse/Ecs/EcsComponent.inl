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

            template<typename ContextType>
            inline ComponentGroup<ContextType>::ComponentGroup(const size_t componentsPerEntity) :
                componentsPerEntity(componentsPerEntity),
                entityCount(0)
            { }

            template<typename ... Components>
            inline constexpr size_t GetComponenetsSize()
            {
                size_t size = 0;
                ForEachTemplateType<Components...>([&size](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    size += sizeof(Type);
                });

                return size;
            }

            template<typename ... Components>
            inline constexpr std::array<ComponentOffsetItem, sizeof...(Components)> CreateComponentOffsets()
            {
                struct Item
                {
                    ComponentTypeId componentTypeId;
                    size_t size;

                    bool operator <(const Item& item) const
                    {
                        return componentTypeId < item.componentTypeId;
                    }
                };

                std::vector<Item> items;
                ForEachTemplateType<Components...>([&items](auto type)
                {
                    using Type = typename decltype(type)::Type;
                    items.push_back({ Type::componentTypeId, sizeof(Type) });
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

                return std::move(offsets);
            }

          
            template<typename Comp, typename ... Components>
            inline size_t GetComponentIndexOfTypes()
            {
           
                // TODO: static_assert with some cool meta programming, like TemplatePackContains<Comp, OfComps...>

                std::vector<ComponentTypeId> componentIds;

                ForEachTemplateType<Components...>([&componentIds](auto type)
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

                throw Exception("Comp is not in the set of Components.");
            }

        }

    }

}
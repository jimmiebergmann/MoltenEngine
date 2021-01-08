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

#include <stack>

namespace Molten
{

    // Byspass tree implementations.
    template<typename T>
    template<typename TLaneType>
    inline typename BypassTree<T>::template Lane<TLaneType> BypassTree<T>::GetLane()
    {
        return m_list.template GetLane<TLaneType>();
    }

    template<typename T>
    template<typename TLaneType>
    inline typename BypassTree<T>::template ConstLane<TLaneType> BypassTree<T>::GetLane() const
    {
        return m_list.template GetLane<TLaneType>();
    }

    template<typename T>
    template<typename TLaneType, template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    inline typename BypassTree<T>::template Iterator<typename TLaneType::LaneType> BypassTree<T>::Insert(TLaneType& lane, TIterator<IsItConst, IsItReverse, TItLaneType, Item> position, const Type& value)
    {
        auto& parent = lane.GetList();
        return lane.Insert(position, std::move( Item{ parent, value } ));
    }
    template<typename T>
    template<typename TLaneType, template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    inline typename BypassTree<T>::template Iterator<typename TLaneType::LaneType> BypassTree<T>::Insert(TLaneType& lane, TIterator<IsItConst, IsItReverse, TItLaneType, Item> position, Type&& value)
    {
        auto& parent = lane.GetList();
        return lane.Insert(position, std::move(Item{ parent, std::move(value) }) );
    }

    template<typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    inline TIterator<false, IsItReverse, TItLaneType, typename BypassTree<T>::Item> BypassTree<T>::Erase(TIterator<IsItConst, IsItReverse, TItLaneType, Item> it)
    {
        auto& list = it.GetList();
        return list.Erase(it);
    }

    template<typename T>
    template<typename TLaneType, typename TCallback>
    inline void BypassTree<T>::ForEachPreorder(TCallback&& callback)
    {
        struct State
        {
            Iterator<TLaneType> current;
            Iterator<TLaneType> end;
        };

        std::stack<State> stack;
        auto lane = GetLane<TLaneType>();
        stack.push(State{ lane.begin(), lane.end() });

        while (!stack.empty())
        {
            auto& state = stack.top();
            if (state.current == state.end)
            {
                stack.pop();
                continue;
            }

            auto& item = *state.current;

            if constexpr (std::is_same_v<std::invoke_result_t<decltype(callback), decltype(item.GetValue())>, bool>)
            {
                if (!callback(item.GetValue()))
                {
                    return;
                }
            }
            else
            {
                callback(item.GetValue());
            }

            lane = item.GetChildren().template GetLane<TLaneType>();
            if (lane.begin() != lane.end())
            {
                stack.push(State{ lane.begin(), lane.end() });
            }

            ++state.current;
        }
    }

    template<typename T>
    template<typename TLaneType, typename TCallback, typename TPostCallback>
    inline void BypassTree<T>::ForEachPreorder(TCallback&& preCallback, TPostCallback&& postCallback)
    {
        struct State
        {
            Item* parent;
            Iterator<TLaneType> current;
            Iterator<TLaneType> end;
        };

        std::stack<State> stack;
        auto rootLane = GetLane<TLaneType>();
        stack.push({ nullptr, rootLane.begin(), rootLane.end() });
    
        while (!stack.empty())
        {
            auto& state = stack.top();

            if (state.current == state.end)
            {
                if (state.parent)
                {
                    if constexpr (std::is_same_v<std::invoke_result_t<decltype(postCallback), decltype(state.parent->GetValue())>, bool>)
                    {
                        if (!postCallback(state.parent->GetValue()))
                        {
                            return;
                        }
                    }
                    else
                    {
                        postCallback(state.parent->GetValue());
                    }
                }
                stack.pop();
                continue;
            }

            auto& item = *state.current;

            if constexpr (std::is_same_v<std::invoke_result_t<decltype(preCallback), decltype(item.GetValue())>, bool>)
            {
                if (!preCallback(item.GetValue()))
                {
                    return;
                }
            }
            else
            {
                preCallback(item.GetValue());
            }

            auto lane = item.GetChildren().template GetLane<TLaneType>();
            if (lane.begin() != lane.end())
            {
                stack.push(State{ &item, lane.begin(), lane.end() });
            }
            else
            {
                if constexpr (std::is_same_v<std::invoke_result_t<decltype(postCallback), decltype(item.GetValue())>, bool>)
                {
                    if (!postCallback(item.GetValue()))
                    {
                        return;
                    }
                }
                else
                {
                    postCallback(item.GetValue());
                }
            }

            ++state.current;
        }
    }

    template<typename T>
    template<typename TLaneType, typename TCallback>
    inline void BypassTree<T>::ForEachReversePreorder(TCallback&& callback)
    {
        struct State
        {
            Item* parent;
            ReverseIterator<TLaneType> current;
            ReverseIterator<TLaneType> end;
        };

        std::stack<State> stack;
        auto rootLane = GetLane<TLaneType>();
        stack.push({ nullptr, rootLane.rbegin(), rootLane.rend() });

        while (!stack.empty())
        {
            auto& state = stack.top();

            if (state.current == state.end)
            {
                if (state.parent)
                {
                    if constexpr (std::is_same_v<std::invoke_result_t<decltype(callback), decltype(state.parent->GetValue())>, bool>)
                    {
                        if (!callback(state.parent->GetValue()))
                        {
                            return;
                        }
                    }
                    else
                    {
                        callback(state.parent->GetValue());
                    }
                }
                stack.pop();
                continue;
            }

            auto& item = *state.current;

            auto lane = item.GetChildren().template GetLane<TLaneType>();
            if (lane.rbegin() != lane.rend())
            {
                stack.push(State{ &item, lane.rbegin(), lane.rend() });
            }
            else
            {
                if constexpr (std::is_same_v<std::invoke_result_t<decltype(callback), decltype(item.GetValue())>, bool>)
                {
                    if (!callback(item.GetValue()))
                    {
                        return;
                    }
                }
                else
                {
                    callback(item.GetValue());
                }
            }

            ++state.current;
        }
    }


    // Bypass tree item implementations.
    template<typename T>
    BypassTreeItem<T>::BypassTreeItem(List& parent, const T& value) :
        m_value(value),
        m_parent(&parent)
    {}

    template<typename T>
    BypassTreeItem<T>::BypassTreeItem(List& parent, T&& value) :
        m_value(value),
        m_parent(&parent)
    {}

    template<typename T>
    BypassTreeItem<T>::BypassTreeItem(BypassTreeItem<T>&& treeItem) noexcept :
        m_value(std::move(treeItem.m_value)),
        m_parent(treeItem.m_parent),
        m_children(std::move(treeItem.m_children))
    {
        treeItem.m_parent = nullptr;
    }

    template<typename T>
    BypassTreeItem<T>& BypassTreeItem<T>::operator = (BypassTreeItem<T>&& treeItem) noexcept
    {
        m_value = std::move(treeItem.m_value);
        m_parent = treeItem.m_parent;
        treeItem.m_parent = nullptr;
        m_children = std::move(treeItem.m_children);
        return *this;
    }

    template<typename T>
    T& BypassTreeItem<T>::GetValue()
    {
        return m_value;
    }
    template<typename T>
    const T& BypassTreeItem<T>::GetValue() const
    {
        return m_value;
    }

    template<typename T>
    typename BypassTreeItem<T>::List& BypassTreeItem<T>::GetParent()
    {
        return *m_parent;
    }
    template<typename T>
    const typename BypassTreeItem<T>::List& BypassTreeItem<T>::GetParent() const
    {
        return *m_parent;
    }

    template<typename T>
    BypassTree<T>& BypassTreeItem<T>::GetChildren()
    {
        return m_children;
    }
    template<typename T>
    const BypassTree<T>& BypassTreeItem<T>::GetChildren() const
    {
        return m_children;
    }

}
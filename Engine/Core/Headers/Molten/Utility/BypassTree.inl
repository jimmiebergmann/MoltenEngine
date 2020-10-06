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

    // Bypass tree implementations.
    template<typename T>
    inline BypassTree<T>::BypassTree() :
        m_rootItem()
    {}

    template<typename T>
    inline BypassTree<T>::BypassTree(const Type& value) :
        m_rootItem(nullptr, value)
    {}

    template<typename T>
    inline BypassTree<T>::BypassTree(Type&& value) :
        m_rootItem(nullptr, std::move(value))
    {}

    template<typename T>
    template<typename TLaneType>
    inline typename BypassTree<T>::template Lane<TLaneType> BypassTree<T>::GetLane()
    {
        return m_rootItem.template GetLane<TLaneType>();
    }
    template<typename T>
    template<typename TLaneType>
    inline typename BypassTree<T>::template ConstLane<TLaneType> BypassTree<T>::GetLane() const
    {
        return m_rootItem.template GetLane<TLaneType>();
    }

    template<typename T>
    inline typename BypassTree<T>::Item& BypassTree<T>::GetItem()
    {
        return m_rootItem;
    }

    template<typename T>
    inline const typename BypassTree<T>::Item& BypassTree<T>::GetItem() const
    {
        return m_rootItem;
    }

    /*template<typename T>
    template<typename TLaneType, template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassTree<T>::template Iterator<TLaneType> BypassTree<T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, const Type& value)
    {
        MOLTEN_DEBUG_ASSERT(position.IsEmpty() == false, "Cannot insert with empty iterator as position.");

        auto* parent = position.m_item;
        ////auto newIt = parent->Insert(position.m_listIterator, value);
        //return Iterator<TLaneType>{ parent, newIt };

        return Iterator<TLaneType>{};
    }
    template<typename T>
    template<typename TLaneType, template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassTree<T>::template Iterator<TLaneType> BypassTree<T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, Type&& value)
    {
        return Iterator<TLaneType>{};
    }*/

    template<typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline TIterator<false, TItLaneType, T> BypassTree<T>::Erase(TIterator<IsItConst, TItLaneType, Type> it)
    {
        auto* item = it.m_item;
        auto& children = item->m_children;
        auto nextIt = children.Erase(it.m_listIterator);

        return TIterator<false, TItLaneType, T>(item, nextIt);
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

        callback(m_rootItem.GetValue());

        while (!stack.empty())
        {
            auto& state = stack.top();
            if (state.current == state.end)
            {
                stack.pop();
                continue;
            }

            auto& item = *state.current;
            callback(item.GetValue());
            
            lane = item.template GetLane<TLaneType>();
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
            Item& item;
            Iterator<TLaneType> current;
            Iterator<TLaneType> end;
        };

        std::stack<State> stack;
        auto lane = GetLane<TLaneType>();

        auto& rootValue = m_rootItem.GetValue();
        stack.push(State{ m_rootItem, lane.begin(), lane.end() });

        preCallback(rootValue);

        while (!stack.empty())
        {
            auto& state = stack.top();
            if (state.current == state.end)
            {
                postCallback(state.item);
                stack.pop();
                continue;
            }

            auto& item = *state.current;
            preCallback(item.GetValue());

            lane = item.template GetLane<TLaneType>();
            if (lane.begin() != lane.end())
            {
                stack.push(State{ item, lane.begin(), lane.end() });
            }
            else
            {
                postCallback(state.item);
            }

            ++state.current;
        }

        postCallback(rootValue);
    }

    // Bypass tree lane interface implementations.
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>::BypassTreeLaneInterface() :
        m_item(nullptr),
        m_listLane()
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>::BypassTreeLaneInterface(Item* item, ListLane listLane) :
        m_item(item),
        m_listLane(listLane)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>::BypassTreeLaneInterface(
        typename std::conditional<
        !IsConst, const BypassTreeLaneInterface<false, LaneType, Type>, const InvalidCopyType1>::
        type& lane
    ) :
        m_item(lane.m_item),
        m_listLane(lane.m_listLane)
    {}
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>& BypassTreeLaneInterface<IsConst, TLaneType, T>::operator = (
        typename std::conditional<
        !IsConst, const BypassTreeLaneInterface<false, LaneType, Type>, const InvalidCopyType1>::
        type& lane)
    {
        m_item = lane.m_item;
        m_listLane = lane.m_listLane;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>::BypassTreeLaneInterface(
        typename std::conditional<
        IsConst, const BypassTreeLaneInterface<false, LaneType, Type>, const InvalidCopyType2>::
        type& lane
    ) :
        m_item(lane.m_item),
        m_listLane(lane.m_listLane)
    {}
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>& BypassTreeLaneInterface<IsConst, TLaneType, T>::operator = (
        typename std::conditional<
        IsConst, const BypassTreeLaneInterface<false, LaneType, Type>, const InvalidCopyType2>::
        type& lane)
    {
        m_item = lane.m_item;
        m_listLane = lane.m_listLane;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>::BypassTreeLaneInterface(
        typename std::conditional<
        IsConst, const BypassTreeLaneInterface<true, LaneType, Type>, const InvalidCopyType3>::
        type& lane
    ) :
        m_item(lane.m_item),
        m_listLane(lane.m_listLane)
    {}
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeLaneInterface<IsConst, TLaneType, T>& BypassTreeLaneInterface<IsConst, TLaneType, T>::operator = (
        typename std::conditional<
        IsConst, const BypassTreeLaneInterface<true, LaneType, Type>, const InvalidCopyType3>::
        type& lane)
    {
        m_item = lane.m_item;
        m_listLane = lane.m_listLane;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassTreeLaneInterface<IsConst, TLaneType, T>::IsEmpty() const
    {
        return m_listLane.IsEmpty();
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline size_t BypassTreeLaneInterface<IsConst, TLaneType, T>::GetSize() const
    {
        return m_listLane.GetSize();
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassTreeLaneInterface<IsConst, TLaneType, T>::PushBack(const Type& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        Item* parent = m_item->m_parent;
        m_listLane.PushBack(Item(parent, value));
    }
    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassTreeLaneInterface<IsConst, TLaneType, T>::PushBack(Type&& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        Item* parent = m_item->m_parent;
        m_listLane.PushBack(Item(parent, std::move(value)));
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassTreeLaneInterface<IsConst, TLaneType, T>::PushFront(const Type& value)
    {
        static_assert(!IsConst, "Cannot push front to const lane.");

        Item* parent = m_item->m_parent;
        m_listLane.PushFront(Item(parent, value));
    }
    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassTreeLaneInterface<IsConst, TLaneType, T>::PushFront(Type&& value)
    {
        static_assert(!IsConst, "Cannot push front to const lane.");

        Item* parent = m_item->m_parent;
        m_listLane.PushFront(Item(parent, std::move(value)));
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator
        BypassTreeLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, const Type& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        Item* parent = m_item->m_parent;
        auto& listIterator = position.m_listIterator;
        auto it = m_listLane.Insert(listIterator, Item(parent, value));
        return Iterator(m_item, it);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator
        BypassTreeLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, Type&& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        Item* parent = m_item->m_parent;
        auto& listIterator = position.m_listIterator;
        auto it = m_listLane.Insert(listIterator, Item(parent, std::move(value)));
        return Iterator(m_item, it);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator> 
        BypassTreeLaneInterface<IsConst, TLaneType, T>::begin()
    {
        return Iterator(m_item, m_listLane.begin());
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::ConstIterator> 
        BypassTreeLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        return ConstIterator(m_item, m_listLane.begin());
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator> 
        BypassTreeLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        return Iterator(m_item, m_listLane.begin());
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator>
        BypassTreeLaneInterface<IsConst, TLaneType, T>::end()
    {
        return Iterator(m_item, m_listLane.end());
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::ConstIterator>
        BypassTreeLaneInterface<IsConst, TLaneType, T>::end() const
    {
        return ConstIterator(m_item, m_listLane.end());
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<IsConstLane, typename BypassTreeLaneInterface<IsConst, TLaneType, T>::Iterator>
        BypassTreeLaneInterface<IsConst, TLaneType, T>::end() const
    {
        return Iterator(m_item, m_listLane.end());
    }


    // Bypass tree iterator interface implementations.
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeIteratorInterface<IsConst, TLaneType, T>::BypassTreeIteratorInterface() :
        m_item(nullptr),
        m_listIterator()
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeIteratorInterface<IsConst, TLaneType, T>::BypassTreeIteratorInterface(ItemPointer item, ListIterator listIterator) :
        m_item(item),
        m_listIterator(listIterator)
    {}
    
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeIteratorInterface<IsConst, TLaneType, T>::BypassTreeIteratorInterface(const BypassTreeIteratorInterface<IsConst, BypassTreePartialLane, Type>& it) :
        m_item(it.m_item),
        m_listIterator(it.m_listIterator)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassTreeIteratorInterface<IsConst, TLaneType, T>& 
        BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator = (const BypassTreeIteratorInterface<IsConst, BypassTreePartialLane, Type>& it)
    {
        m_item = it.m_item;
        m_listIterator = it.m_listIterator;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassTreeIteratorInterface<IsConst, TLaneType, T>::IsEmpty() const
    {
        return m_listIterator.IsEmpty();
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstIterator>
    inline std::enable_if_t<!IsConstIterator, typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::ItemReference> BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator *()
    {
        return *m_listIterator;
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstIterator>
    inline std::enable_if_t<IsConstIterator, typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::ItemReference> BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator *() const
    {
        return *m_listIterator;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::Iterator& BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator ++ () // Pre
    {
        ++m_listIterator;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::Iterator& BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator -- () // Pre
    {
        --m_listIterator;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::Iterator BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator ++ (int) // Post
    {
        auto oldIterator = m_listIterator++;
        return Iterator{ m_item, oldIterator };
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassTreeIteratorInterface<IsConst, TLaneType, T>::Iterator BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator -- (int) // Post
    {
        auto oldIterator = m_listIterator--;
        return Iterator{ m_item, oldIterator };
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator == (const BypassTreeIteratorInterface<IsConst, TLaneType, T>& rhs) const
    {
        return m_listIterator == rhs.m_listIterator;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassTreeIteratorInterface<IsConst, TLaneType, T>::operator != (const BypassTreeIteratorInterface<IsConst, TLaneType, T>& rhs) const
    {
        return m_listIterator != rhs.m_listIterator;
    }


    // Bypass tree item implementations.
    template<typename T>
    inline BypassTreeItem<T>::BypassTreeItem() :
        m_parent(nullptr),
        m_value {}
    {}

    template<typename T>
    inline BypassTreeItem<T>::BypassTreeItem(Item* parent, const Type& value) :
        m_parent(parent),
        m_value(value)
    {}

    template<typename T>
    inline BypassTreeItem<T>::BypassTreeItem(Item* parent, Type&& value) :
        m_parent(parent),
        m_value(std::move(value))
    {}

    template<typename T>
    template<typename TLaneType>
    inline typename BypassTreeItem<T>::template Lane<TLaneType> BypassTreeItem<T>::GetLane()
    {
        typename BypassList<ListItem>::template Lane<TLaneType> listLane = m_children.template GetLane<TLaneType>();
        return Lane<TLaneType>{this, listLane};
    }
    template<typename T>
    template<typename TLaneType>
    inline typename BypassTreeItem<T>::template ConstLane<TLaneType> BypassTreeItem<T>::GetLane() const
    {
        typename BypassList<ListItem>::template ConstLane<TLaneType> listLane = m_children.template GetLane<TLaneType>();
        return ConstLane<TLaneType>{this, listLane};
    }

    template<typename T>
    inline typename BypassTreeItem<T>::Type& BypassTreeItem<T>::GetValue()
    {
        return m_value;
    }
    template<typename T>
    inline const typename BypassTreeItem<T>::Type& BypassTreeItem<T>::GetValue() const
    {
        return m_value;
    }

    template<typename T>
    inline bool BypassTreeItem<T>::HasParent() const
    {
        return m_parent != nullptr;
    }

    template<typename T>
    inline typename BypassTreeItem<T>::Item& BypassTreeItem<T>::GetParent()
    {
        return *m_parent;
    }
    template<typename T>
    inline const typename BypassTreeItem<T>::Item& BypassTreeItem<T>::GetParent() const
    {
        return *m_parent;
    }

}
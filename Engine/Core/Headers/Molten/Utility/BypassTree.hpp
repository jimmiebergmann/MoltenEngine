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

#ifndef MOLTEN_CORE_UTILITY_BYPASSTREE_HPP
#define MOLTEN_CORE_UTILITY_BYPASSTREE_HPP

#include "Molten/Utility/BypassList.hpp"

namespace Molten
{

    template<typename T>
    class BypassTree;

    template<typename T>
    class BypassTreeItem;

    using BypassTreeNormalLane = BypassListNormalLane;
    using BypassTreePartialLane = BypassListPartialLane;


    /* This is a tree container, based of BypassList. */
    template<typename T>
    class BypassTree
    {

    public:

        using Type = T;
        using Item = BypassTreeItem<T>;
        using List = BypassList<Item>;
        template<typename TLaneType>
        using Lane = typename List::template Lane<TLaneType>;
        template<typename TLaneType>
        using ConstLane = typename List::template ConstLane<TLaneType>;

        using NormalLaneType = BypassListNormalLane;
        using PartialLaneType = BypassListPartialLane;
        using NormalLane = Lane<NormalLaneType>;
        using PartialLane = Lane<PartialLaneType>;
        using NormalConstLane = ConstLane<NormalLaneType>;
        using PartialConstLane = ConstLane<PartialLaneType>;

        template<typename TLaneType>
        using Iterator = BypassListIterator<TLaneType, Item>;
        template<typename TLaneType>
        using ConstIterator = BypassListConstIterator<TLaneType, Item>;
        template<typename TLaneType>
        using ReverseIterator = BypassListReverseIterator<TLaneType, Item>;
        template<typename TLaneType>
        using ReverseConstIterator = BypassListReverseConstIterator<TLaneType, Item>;

        BypassTree() = default;

        template<typename TLaneType>
        Lane<TLaneType> GetLane();
        template<typename TLaneType>
        ConstLane<TLaneType> GetLane() const;

        template<typename TLaneType, template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        Iterator<typename TLaneType::LaneType> Insert(TLaneType& lane, TIterator<IsItConst, IsItReverse, TItLaneType, Item> position, const Type& value);
        template<typename TLaneType, template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        Iterator<typename TLaneType::LaneType> Insert(TLaneType& lane, TIterator<IsItConst, IsItReverse, TItLaneType, Item> position, Type&& value);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        TIterator<false, IsItReverse, TItLaneType, Item> Erase(TIterator<IsItConst, IsItReverse, TItLaneType, Item> it);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse>
        void EnableInPartialLane(TIterator<IsItConst, IsItReverse, NormalLaneType, Item> it);
        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        void DisableInPartialLane(TIterator<IsItConst, IsItReverse, TItLaneType, Item> it);

        template<typename TLaneType, typename TCallback>
        void ForEachPreorder(TCallback&& callback);
        template<typename TLaneType, typename TCallback, typename TPostCallback>
        void ForEachPreorder(TCallback&& preCallback, TPostCallback&& postCallback);

        template<typename TLaneType, typename TCallback>
        void ForEachReversePreorder(TCallback&& callback);

    private:

        List m_list;

    };

    template<typename T>
    class BypassTreeItem
    {

    public:

        using List = BypassList<BypassTreeItem<T>>;

        BypassTreeItem(List& parent, const T& value);
        BypassTreeItem(List& parent, T&& value);

        BypassTreeItem(BypassTreeItem&& treeItem) noexcept;
        BypassTreeItem& operator = (BypassTreeItem&& treeItem) noexcept;

        BypassTreeItem(const BypassTreeItem& treeItem) = delete;
        BypassTreeItem& operator = (const BypassTreeItem& treeItem) = delete;

        T& GetValue();
        const T& GetValue() const;

        List& GetParent();
        const List& GetParent() const;

        BypassTree<T>& GetChildren();
        const BypassTree<T>& GetChildren() const;

    private:

        T m_value;
        List* m_parent;
        BypassTree<T> m_children;

    };

}

#include "Molten/Utility/BypassTree.inl"

#endif
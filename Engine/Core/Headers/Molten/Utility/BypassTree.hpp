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

    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeLaneInterface;

    template<typename TLaneType, typename T>
    using BypassTreeLane = BypassTreeLaneInterface<false, TLaneType, T>;

    template<typename TLaneType, typename T>
    using BypassTreeConstLane = BypassTreeLaneInterface<true, TLaneType, T>;

    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeIteratorInterface;

    template<typename TLaneType, typename T>
    using BypassTreeIterator = BypassTreeIteratorInterface<false, TLaneType, T>;

    template<typename TLaneType, typename T>
    using BypassTreeConstIterator = BypassTreeIteratorInterface<true, TLaneType, T>;

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
        using Tree = BypassTree<Type>;
        template<typename TLaneType>
        using Lane = BypassTreeLane<TLaneType, Type>;
        template<typename TLaneType>
        using ConstLane = BypassTreeConstLane<TLaneType, Type>;
        using NormalLaneType = BypassTreeNormalLane;
        using PartialLaneType = BypassTreePartialLane;
        using NormalLane = Lane<NormalLaneType>;
        using PartialLane = Lane<PartialLaneType>;
        using NormalConstLane = ConstLane<NormalLaneType>;
        using PartialConstLane = ConstLane<PartialLaneType>;
        template<typename TLaneType>
        using Iterator = BypassTreeIterator<TLaneType, Type>;
        template<typename TLaneType>
        using ConstIterator = BypassTreeConstIterator<TLaneType, Type>;
        using Item = BypassTreeItem<Type>;

        BypassTree();
        BypassTree(const Type& value);
        BypassTree(Type&& value);

        template<typename TLaneType>
        Lane<TLaneType> GetLane();
        template<typename TLaneType>
        ConstLane<TLaneType> GetLane() const;

        Item& GetItem();
        const Item& GetItem() const;

        /*template<typename TLaneType, template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
        Iterator<TLaneType> Insert(TIterator<IsItConst, TItLaneType, Type> position, const Type& value);
        template<typename TLaneType, template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
        Iterator<TLaneType> Insert(TIterator<IsItConst, TItLaneType, Type> position, Type&& value);*/

        template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
        TIterator<false, TItLaneType, Type> Erase(TIterator<IsItConst, TItLaneType, Type> it);

        template<typename TLaneType, typename TCallback>
        void ForEachPreorder(TCallback&& callback);
        template<typename TLaneType, typename TCallback, typename TPostCallback>
        void ForEachPreorder(TCallback&& preCallback, TPostCallback&& postCallback);

    private:

        Item m_rootItem;

    };


    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeLaneInterface
    {

        static_assert(std::is_same_v<TLaneType, BypassTreeNormalLane> || std::is_same_v<TLaneType, BypassTreePartialLane>, "Invalid bypass lane type.");

    public:

        using LaneType = TLaneType;
        using Type = T;
        using Tree = BypassTree<Type>;
        using NormalLaneType = BypassTreeNormalLane;
        using PartialLaneType = BypassTreePartialLane;
        using Iterator = typename std::conditional_t<IsConst, BypassTreeConstIterator<LaneType, Type>, BypassTreeIterator<LaneType, Type>>;
        using ConstIterator = BypassTreeConstIterator<LaneType, Type>;
        using Item = typename std::conditional_t<IsConst, const BypassTreeItem<Type>, BypassTreeItem<Type>>;
        using ListLaneItem = BypassTreeItem<Type>;
        using ListLane = BypassListLaneInterface<IsConst, LaneType, ListLaneItem>;

        BypassTreeLaneInterface();
        BypassTreeLaneInterface(Item* item, ListLane listLane);
           
        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassTreeLaneInterface(const BypassTreeLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassTreeLaneInterface& operator =(const BypassTreeLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane);

        bool IsEmpty() const;

        size_t GetSize() const;

        void PushBack(const Type& value);
        void PushBack(Type&& value);

        void PushFront(const Type& value);
        void PushFront(Type&& value);

        template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
        Iterator Insert(TIterator<IsItConst, TItLaneType, Type> position, const Type& value);
        template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
        Iterator Insert(TIterator<IsItConst, TItLaneType, Type> position, Type&& value);

        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, Iterator> begin();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ConstIterator> begin() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, Iterator> begin() const;

        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, Iterator> end();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ConstIterator> end() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, Iterator> end() const;

    private:

        Item* m_item;
        ListLane m_listLane;

        template<bool IsConst2, typename TLaneType2, typename T2>
        friend class BypassTreeLaneInterface;

    };

    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeIteratorInterface
    {

        static_assert(std::is_same_v<TLaneType, BypassTreeNormalLane> || std::is_same_v<TLaneType, BypassTreePartialLane>, "Invalid bypass lane type.");

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = BypassTreeItem<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::conditional_t<IsConst, const value_type*, value_type*>;
        using reference = typename std::conditional_t<IsConst, const value_type&, value_type&>;

        using LaneType = TLaneType;
        using Type = T;
        using Item = value_type;
        using ItemPointer = pointer;
        using ItemReference = reference;
        using Iterator = BypassTreeIteratorInterface<IsConst, LaneType, Type>;
        using ListIterator = BypassListIteratorInterface<IsConst, LaneType, Item>;
        
        BypassTreeIteratorInterface();
        BypassTreeIteratorInterface(ItemPointer item, ListIterator listIterator);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassTreeIteratorInterface(const BypassTreeIteratorInterface<IsOtherConst, TLaneTypeOther, Type>& it);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassTreeIteratorInterface& operator =(const BypassTreeIteratorInterface<IsOtherConst, TLaneTypeOther, Type>& it);

        bool IsEmpty() const;

        template<bool IsConstIterator = IsConst>
        std::enable_if_t<!IsConstIterator, ItemReference> operator *();
        template<bool IsConstIterator = IsConst>
        std::enable_if_t<IsConstIterator, ItemReference> operator *() const;

        Iterator& operator ++ ();
        Iterator& operator -- ();
        Iterator operator ++ (int);
        Iterator operator -- (int);
        bool operator == (const Iterator& rhs) const;
        bool operator != (const Iterator& rhs) const;

    private:

        ItemPointer m_item;
        ListIterator m_listIterator;

        friend class BypassTree<T>;
        template<bool IsConst2, typename TLaneType2, typename T2>
        friend class BypassTreeIteratorInterface;
        template<bool IsConst2, typename TLaneType2, typename T2>
        friend class BypassTreeLaneInterface;

    };


    template<typename T>
    class BypassTreeItem
    {

    public:

        using Type = T;
        template<typename TLaneType>
        using Lane = BypassTreeLane<TLaneType, Type>;
        template<typename TLaneType>
        using ConstLane = BypassTreeConstLane<TLaneType, Type>;
        using Item = BypassTreeItem<Type>;
        using ListItem = BypassTreeItem<Type>;
        using List = BypassList<ListItem>;
        
        BypassTreeItem();
        BypassTreeItem(Item* parent, const Type& value);
        BypassTreeItem(Item* parent, Type&& value);

        template<typename TLaneType>
        Lane<TLaneType> GetLane();
        template<typename TLaneType>
        ConstLane<TLaneType> GetLane() const;

        Type& GetValue();
        const Type& GetValue() const;

        bool HasParent() const;

        Item& GetParent();
        const Item& GetParent() const;

    private:

        Item* m_parent;
        List m_children;
        Type m_value;

        friend class BypassTree<T>;
        template<bool IsConst, typename TLaneType, typename T2>
        friend class BypassTreeLaneInterface;

    };

}

#include "Molten/Utility/BypassTree.inl"

#endif
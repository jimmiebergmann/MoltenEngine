/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_UTILITY_BYPASSLIST_HPP
#define MOLTEN_CORE_UTILITY_BYPASSLIST_HPP

#include "Molten/Core.hpp"
#include <tuple>
#include <type_traits>

namespace Molten
{

    template<typename T>
    class BypassList;

    template<bool IsConst, typename TLaneType, typename T>
    class BypassListLaneInterface;

    template<typename TLaneType, typename T>
    using BypassListLane = BypassListLaneInterface<false, TLaneType, T>;

    template<typename TLaneType, typename T>
    using BypassListConstLane = BypassListLaneInterface<true, TLaneType, T>;

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    class BypassListIteratorInterface;

    template<typename TLaneType, typename T>
    using BypassListIterator = BypassListIteratorInterface<false, false, TLaneType, T>;
    template<typename TLaneType, typename T>
    using BypassListReverseIterator = BypassListIteratorInterface<false, true, TLaneType, T>;

    template<typename TLaneType, typename T>
    using BypassListConstIterator = BypassListIteratorInterface<true, false, TLaneType, T>;
    template<typename TLaneType, typename T>
    using BypassListReverseConstIterator = BypassListIteratorInterface<true, true, TLaneType, T>;

    template<typename T>
    struct BypassListItem;

    template<typename TLaneType, typename T>
    struct BypassListDataLane;

    struct BypassListNormalLane {};

    struct BypassListPartialLane {};

    template<typename T>
    using BypassListDataLanes = std::tuple<BypassListDataLane<BypassListNormalLane, T>, BypassListDataLane<BypassListPartialLane, T>>;


    /*
    * A bypass list is specific purpose linked list container with two possible lanes: normal lane and partial lane.
    * Normal lane links together all items in the list and should be used for traversing all items.
    * Partial lane links together selected items by the user. Use this lane for iteration of selected items.
    * Items are placed in the partial lane at item creation, or by moving items into this lane.
    * 
    * This container is suitable for storing a list of items, where a fast iteration of enabled items is needed,
    * for example an GUI tree, where some items are disabled and therefore not present in the partial lane.
    */
    template<typename T>
    class BypassList
    {

    public:

        using Type = T;
        template<typename TLaneType>
        using List = BypassList<Type>;
        template<typename TLaneType>
        using Lane = BypassListLane<TLaneType, Type>;
        template<typename TLaneType>
        using ConstLane = BypassListConstLane<TLaneType, Type>;
        using NormalLaneType = BypassListNormalLane;
        using PartialLaneType = BypassListPartialLane;
        using NormalLane = Lane<NormalLaneType>;
        using PartialLane = Lane<PartialLaneType>;
        using NormalConstLane = ConstLane<NormalLaneType>;
        using PartialConstLane = ConstLane<PartialLaneType>;
        
        using Item = BypassListItem<Type>;
        template<typename TLaneType>
        using DataLane = BypassListDataLane<TLaneType, Type>;

        template<typename TLaneType>
        using Iterator = BypassListIterator<TLaneType, Type>;
        template<typename TLaneType>
        using ConstIterator = BypassListConstIterator<TLaneType, Type>;        
        template<typename TLaneType>
        using ReverseIterator = BypassListReverseIterator<TLaneType, Type>;
        template<typename TLaneType>
        using ReverseConstIterator = BypassListReverseConstIterator<TLaneType, Type>;
           

        BypassList();
        ~BypassList();

        BypassList(const BypassList& list) = delete;
        BypassList& operator =(const BypassList& list) = delete;

        BypassList(BypassList&& list) noexcept;
        BypassList& operator =(BypassList&& list) noexcept;

        template<typename TLaneType>
        Lane<TLaneType> GetLane();
        template<typename TLaneType>
        ConstLane<TLaneType> GetLane() const;

        size_t GetSize() const;

        void Clear();  

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        TIterator<false, IsItReverse, TItLaneType, Type> Erase(TIterator<IsItConst, IsItReverse, TItLaneType, Type> it);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse>
        void EnableInPartialLane(TIterator<IsItConst, IsOtherReverse, BypassListNormalLane, Type> it);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
        void DisableInPartialLane(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> it);

    private:

        void InternalClearAllItems();

        Item* m_beginItem;
        Item* m_endItem;
        BypassListDataLanes<Type> m_dataLanes;

    };


    template<bool IsConst, typename TLaneType, typename T>
    class BypassListLaneInterface
    {

        static_assert(std::is_same_v<TLaneType, BypassListNormalLane> || std::is_same_v<TLaneType, BypassListPartialLane>, "Invalid bypass lane type.");

    public:

        using LaneType = TLaneType;
        using Type = T;
        using List = typename std::conditional_t<IsConst, const BypassList<Type>, BypassList<Type>>;
        using Item = BypassListItem<Type>;
        using NormalLaneType = BypassListNormalLane;
        using PartialLaneType = BypassListPartialLane;
        
        using Iterator = typename std::conditional_t<IsConst, BypassListConstIterator<LaneType, Type>, BypassListIterator<LaneType, Type>>;
        using ConstIterator = BypassListConstIterator<LaneType, Type>;
        using ReverseIterator = typename std::conditional_t<IsConst, BypassListReverseConstIterator<LaneType, Type>, BypassListReverseIterator<LaneType, Type>>;
        using ReverseConstIterator = BypassListReverseConstIterator<LaneType, Type>;

        template<typename TLaneType2>
        using DataLane = BypassListDataLane<TLaneType2, Type>;
        using DataLanes = typename std::conditional_t<IsConst, const BypassListDataLanes<Type>, BypassListDataLanes<Type>>;

        BypassListLaneInterface();
        BypassListLaneInterface(List* list, DataLanes* dataLanes);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassListLaneInterface(const BypassListLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassListLaneInterface& operator =(const BypassListLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane);

        bool IsValid() const; ///< Returns true if lane is constructed by list.

        bool IsEmpty() const; // Returns true if GetSize() == 0 or IsValid() == false.

        size_t GetSize() const;

        List& GetList();

        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, Iterator> begin();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ConstIterator> begin() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, Iterator> begin() const;

        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ReverseIterator> rbegin();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ReverseConstIterator> rbegin() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, ReverseIterator> rbegin() const;
        
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, Iterator> end();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ConstIterator> end() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, Iterator> end() const;

        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ReverseIterator> rend();
        template<bool IsConstLane = IsConst>
        std::enable_if_t<!IsConstLane, ReverseConstIterator> rend() const;
        template<bool IsConstLane = IsConst>
        std::enable_if_t<IsConstLane, ReverseIterator> rend() const;

        void PushBack(const Type& value);
        void PushBack(Type&& value);

        void PushFront(const Type& value);
        void PushFront(Type&& value);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
        Iterator Insert(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> position, const Type& value);
        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
        Iterator Insert(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> position, Type&& value);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
        TIterator<false, IsOtherReverse, TItLaneType, Type> Erase(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> it);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse>
        void EnableInPartialLane(TIterator<IsItConst, IsOtherReverse, BypassListNormalLane, Type> it);
        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
        void DisableInPartialLane(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> it);

    private:

        bool InternalIsInPartialLane(Item* item);

        template<typename TProcessLaneType>
        void InternalPushBack(Item* item);

        template<typename TProcessLaneType>
        void InternalPushFront(Item* item);

        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        void InternalInsertNormal(TIterator<IsItConst, IsItReverse, TItLaneType, Type> position, Item* item);
        template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
        void InternalInsertPartial(TIterator<IsItConst, IsItReverse, TItLaneType, Type> position, Item* item);

        Item* InternalFindPrevPartialItem(Item* item);

        template<typename TProcessLaneType>
        void InternalErase(Item* item);

        List* m_list;
        DataLanes* m_dataLanes;

        template<bool IsConst2, typename TLaneType2, typename T2>
        friend class BypassListLaneInterface;

    };


    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    class BypassListIteratorInterface
    {

        static_assert(std::is_same_v<TLaneType, BypassListNormalLane> || std::is_same_v<TLaneType, BypassListPartialLane>, "Invalid bypass lane type.");

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = typename std::conditional_t<IsConst, const T*, T*>;
        using reference = typename std::conditional_t<IsConst, const T&, T&>;

        using Type = value_type;
        using List = typename std::conditional_t<IsConst, const BypassList<Type>, BypassList<Type>>;
        using LaneType = TLaneType;       
        using Pointer = pointer;
        using Reference = reference;
        using Iterator = BypassListIteratorInterface<IsConst, IsReverse, LaneType, Type>;
        using Item = BypassListItem<Type>;

        BypassListIteratorInterface();
        BypassListIteratorInterface(Item* item);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassListIteratorInterface(const BypassListIteratorInterface<IsOtherConst, IsReverse, TLaneTypeOther, Type>& it);

        template<bool IsOtherConst, typename TLaneTypeOther>
        BypassListIteratorInterface& operator =(const BypassListIteratorInterface<IsOtherConst, IsReverse, TLaneTypeOther, Type>& it);

        bool IsValid() const;

        List& GetList();

        template<bool IsConstIterator = IsConst>
        std::enable_if_t<!IsConstIterator, Reference> operator *();
        template<bool IsConstIterator = IsConst>
        std::enable_if_t<IsConstIterator, Reference> operator *() const;

        template<bool IsConstIterator = IsConst>
        std::enable_if_t<!IsConstIterator, Pointer> operator ->();
        template<bool IsConstIterator = IsConst>
        std::enable_if_t<IsConstIterator, Pointer> operator ->() const;

        Iterator& operator ++ ();
        Iterator& operator -- ();
        Iterator operator ++ (int);
        Iterator operator -- (int);
        bool operator == (const Iterator& rhs) const;
        bool operator != (const Iterator& rhs) const;

    private:

        Item* m_currentItem;

        template<bool IsConst2, bool IsReverse2, typename TLaneType2, typename T2>
        friend class BypassListIteratorInterface;
        template<bool IsConst2, typename TLaneType2, typename T2>
        friend class BypassListLaneInterface;

    };


    template<typename T>
    struct BypassListItem
    {

        using Type = T;
        using List = BypassList<Type>;
        using Item = BypassListItem<Type>;

        BypassListItem(List* list);
        BypassListItem(List* list, const Type& valueCopy);
        BypassListItem(List* list, Type&& valueMove);
        ~BypassListItem();

        BypassListItem(const BypassListItem&) = delete;
        BypassListItem(BypassListItem&&) = delete;
        BypassListItem& operator =(const BypassListItem&) = delete;
        BypassListItem& operator =(BypassListItem&&) = delete;

        struct Content
        {
            Content(const Type& valueCopy);
            Content(Type&& valueMove);
            Type value;
        };

        List* list;
        Content* content;
        BypassListItem* prevNormal;
        BypassListItem* prevPartial;
        BypassListItem* nextNormal;
        BypassListItem* nextPartial;

    };


    template<typename TLaneType, typename T>
    struct BypassListDataLane
    {

        using LaneType = TLaneType;
        using Type = T;
        using List = BypassList<Type>;
        using Item = BypassListItem<Type>;

        BypassListDataLane(Item* rootItem, Item* tailItem);
        BypassListDataLane(BypassListDataLane&& lane) noexcept;
        BypassListDataLane& operator =(BypassListDataLane&& lane) noexcept;

        BypassListDataLane(const BypassListDataLane&) = delete;
        BypassListDataLane& operator =(const BypassListDataLane&) = delete;

        Item* root;
        Item* tail;
        size_t size;

    };


    template<typename TLaneType>
    struct BypassListLaneTraits
    {

        template<typename T>
        static BypassListItem<T>* GetPrev(BypassListItem<T>* item);

        template<typename T>
        static BypassListItem<T>* GetNext(BypassListItem<T>* item);

        template<typename T>
        static void SetPrev(BypassListItem<T>* item, BypassListItem<T>* prev);

        template<typename T>
        static void SetNext(BypassListItem<T>* item, BypassListItem<T>* next);

    };
    
}

#include "Molten/Utility/BypassList.inl"

#endif

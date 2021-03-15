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


namespace Molten
{

    // Bypass list implementations.
    template<typename T>
    BypassList<T>::BypassList() :
        m_beginItem(new Item(this)),
        m_endItem(new Item(this)),
        m_dataLanes(std::make_tuple(BypassListDataLane<BypassListNormalLane, T>{ m_beginItem, m_endItem }, BypassListDataLane<BypassListPartialLane, T>{ m_beginItem, m_endItem }))
    {
        m_beginItem->nextNormal = m_endItem;
        m_beginItem->nextPartial = m_endItem;
        m_endItem->prevNormal = m_beginItem;
        m_endItem->prevPartial = m_beginItem;
    }

    template<typename T>
    BypassList<T>::~BypassList()
    {
        InternalClearAllItems();
    }

    template<typename T>
    BypassList<T>::BypassList(BypassList&& list) noexcept :
        m_beginItem(new Item(this)),
        m_endItem(new Item(this)),
        m_dataLanes(std::make_tuple(BypassListDataLane<BypassListNormalLane, T>{ m_beginItem, m_endItem }, BypassListDataLane<BypassListPartialLane, T>{ m_beginItem, m_endItem }))
    {
        // Normal
        auto* currentItem = m_beginItem;
        auto* moveItem = list.m_beginItem->nextNormal;
        while (moveItem != list.m_endItem)
        {
            moveItem->list = this;

            currentItem->nextNormal = moveItem;
            moveItem->prevNormal = currentItem;
            moveItem = moveItem->nextNormal;
            currentItem = currentItem->nextNormal;
        }
        currentItem->nextNormal = m_endItem;
        m_endItem->prevNormal = currentItem;

        // Partial
        currentItem = m_beginItem;
        moveItem = list.m_beginItem->nextPartial;
        while (moveItem != list.m_endItem)
        {
            currentItem->nextPartial = moveItem;
            moveItem->prevPartial = currentItem;
            moveItem = moveItem->nextPartial;
            currentItem = currentItem->nextPartial;
        }
        currentItem->nextPartial = m_endItem;
        m_endItem->prevPartial = currentItem;

        std::get<BypassListDataLane<BypassListNormalLane, T>>(m_dataLanes).size = std::get<BypassListDataLane<BypassListNormalLane, T>>(list.m_dataLanes).size;
        std::get<BypassListDataLane<BypassListPartialLane, T>>(m_dataLanes).size = std::get<BypassListDataLane<BypassListPartialLane, T>>(list.m_dataLanes).size;
        
        list.m_beginItem->nextNormal = list.m_endItem;
        list.m_beginItem->nextPartial = list.m_endItem;
        list.m_endItem->prevNormal = list.m_beginItem;
        list.m_endItem->prevPartial = list.m_beginItem;
        std::get<BypassListDataLane<BypassListNormalLane, T>>(list.m_dataLanes).size = 0;
        std::get<BypassListDataLane<BypassListPartialLane, T>>(list.m_dataLanes).size = 0;
    }

    template<typename T>
    BypassList<T>& BypassList<T>::operator =(BypassList&& list) noexcept
    {
        InternalClearAllItems();

        m_dataLanes = std::move(list.m_dataLanes);
        m_beginItem = list.m_beginItem;
        m_endItem = list.m_endItem;

        auto* currentItem = m_beginItem->nextNormal;
        while (currentItem != m_endItem)
        {
            currentItem->list = this;
        }

        list.m_beginItem = nullptr;
        list.m_endItem = nullptr;
        return *this;
    }

    template<typename T>
    template<typename TLaneType>
    typename BypassList<T>::template Lane<TLaneType> BypassList<T>::GetLane()
    {
        return Lane<TLaneType>(this, &m_dataLanes);
    }
    template<typename T>
    template<typename TLaneType>
    typename BypassList<T>::template ConstLane<TLaneType> BypassList<T>::GetLane() const
    {
        return ConstLane<TLaneType>(this, &m_dataLanes);
    }

    template<typename T>
    size_t BypassList<T>::GetSize() const
    {
        return std::get<DataLane<NormalLaneType>>(m_dataLanes).size;
    }

    template<typename T>
    void BypassList<T>::Clear()
    {
        auto* currentItem = m_beginItem->nextNormal;
        while (currentItem != m_endItem)
        {
            auto* prevNode = currentItem;
            currentItem = prevNode->nextNormal;
            delete prevNode;
        }
    }

    template<typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    TIterator<false, IsItReverse, TItLaneType, T> BypassList<T>::Erase(TIterator<IsItConst, IsItReverse, TItLaneType, Type> it)
    {
        return GetLane<TItLaneType>().Erase(it);
    }

    template<typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse>
    void BypassList<T>::EnableInPartialLane(TIterator<IsItConst, IsOtherReverse, BypassListNormalLane, T> it)
    {
        return GetLane<NormalLaneType>().EnableInPartialLane(it);
    }

    template<typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
    void BypassList<T>::DisableInPartialLane(TIterator<IsItConst, IsOtherReverse, TItLaneType, T> it)
    {
        return GetLane<NormalLaneType>().DisableInPartialLane(it);
    }

    template<typename T>
    void BypassList<T>::InternalClearAllItems()
    {
        Clear();
        delete m_beginItem;
        delete m_endItem;
        m_endItem = nullptr;
    }

 
    // Bypass list lane interface implementations.
    template<bool IsConst, typename TLaneType, typename T>
    BypassListLaneInterface<IsConst, TLaneType, T>::BypassListLaneInterface() :
        m_list(nullptr),
        m_dataLanes(nullptr)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    BypassListLaneInterface<IsConst, TLaneType, T>::BypassListLaneInterface(List* list, DataLanes* dataLanes) :
        m_list(list),
        m_dataLanes(dataLanes)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsOtherConst, typename TLaneTypeOther>
    BypassListLaneInterface<IsConst, TLaneType, T>::BypassListLaneInterface(const BypassListLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane) :
        m_list(lane.m_list),
        m_dataLanes(lane.m_dataLanes)
    {
        static_assert(IsConst || !IsOtherConst,
            "Cannot construct from const to non-const bypass list lane.");

        static_assert(std::is_same_v<TLaneType, TLaneTypeOther> || std::is_same_v<TLaneTypeOther, BypassListPartialLane>,
            "Cannot construct from normal to partial bypass list lane.");
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsOtherConst, typename TLaneTypeOther>
    BypassListLaneInterface<IsConst, TLaneType, T>&
        BypassListLaneInterface<IsConst, TLaneType, T>::operator = (const BypassListLaneInterface<IsOtherConst, TLaneTypeOther, Type>& lane)
    {
        static_assert(IsConst || !IsOtherConst,
            "Cannot assign from const to non-const bypass list lane.");

        static_assert(std::is_same_v<TLaneType, TLaneTypeOther> || std::is_same_v<TLaneTypeOther, BypassListPartialLane>,
            "Cannot assign from normal to partial bypass list lane.");

        m_list = lane.m_list;
        m_dataLanes = lane.m_dataLanes;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    bool BypassListLaneInterface<IsConst, TLaneType, T>::IsValid() const
    {
        return m_dataLanes == nullptr;
    }

    template<bool IsConst, typename TLaneType, typename T>
    bool BypassListLaneInterface<IsConst, TLaneType, T>::IsEmpty() const
    {
        return m_dataLanes == nullptr || std::get<DataLane<TLaneType>>(*m_dataLanes).size == 0;
    }

    template<bool IsConst, typename TLaneType, typename T>
    size_t BypassListLaneInterface<IsConst, TLaneType, T>::GetSize() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get size from empty bypass list lane.");
        return std::get<DataLane<TLaneType>>(*m_dataLanes).size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    typename BypassListLaneInterface<IsConst, TLaneType, T>::List& BypassListLaneInterface<IsConst, TLaneType, T>::GetList()
    {
        MOLTEN_DEBUG_ASSERT(m_list != nullptr, "Cannot get list from empty bypass list lane.");
        return *m_list;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin iterator from empty bypass list lane.");
        auto* root = std::get<DataLane<TLaneType>>(*m_dataLanes).root;
        auto* nextItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(root);
        return Iterator(nextItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin iterator from empty bypass list lane.");
        auto* root = std::get<DataLane<TLaneType>>(*m_dataLanes).root;
        auto* nextItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(root);
        return ConstIterator(nextItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin const iterator from empty bypass list lane.");
        auto* root = std::get<DataLane<TLaneType>>(*m_dataLanes).root;
        auto* nextItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(root);
        return Iterator(nextItem);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rbegin()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rbegin iterator from empty bypass list lane.");
        auto* tail = std::get<DataLane<TLaneType>>(*m_dataLanes).tail;
        auto* prevItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(tail);
        return ReverseIterator(prevItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rbegin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rbegin iterator from empty bypass list lane.");
        auto* tail = std::get<DataLane<TLaneType>>(*m_dataLanes).tail;
        auto* prevItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(tail);
        return ReverseConstIterator(prevItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rbegin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rbegin const iterator from empty bypass list lane.");
        auto* tail = std::get<DataLane<TLaneType>>(*m_dataLanes).tail;
        auto* prevItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(tail);
        return ReverseIterator(prevItem);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::end()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::end() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end iterator from empty bypass list lane.");
        return ConstIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::end() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end const iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rend()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rend iterator from empty bypass list lane.");
        return ReverseIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rend() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rend iterator from empty bypass list lane.");
        return ReverseConstIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::ReverseIterator> BypassListLaneInterface<IsConst, TLaneType, T>::rend() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get rend const iterator from empty bypass list lane.");
        return ReverseIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }

    template<bool IsConst, typename TLaneType, typename T>
    void BypassListLaneInterface<IsConst, TLaneType, T>::PushBack(const Type& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(m_list, value);

        InternalPushBack<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushBack<PartialLaneType>(newItem);
        }
    }
    template<bool IsConst, typename TLaneType, typename T>
    void BypassListLaneInterface<IsConst, TLaneType, T>::PushBack(Type&& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(m_list, std::move(value));

        InternalPushBack<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushBack<PartialLaneType>(newItem);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    void BypassListLaneInterface<IsConst, TLaneType, T>::PushFront(const Type& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(m_list, value);

        InternalPushFront<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushFront<PartialLaneType>(newItem);
        }
    }
    template<bool IsConst, typename TLaneType, typename T>
    void BypassListLaneInterface<IsConst, TLaneType, T>::PushFront(Type&& value)
    {
        static_assert(!IsConst, "Cannot push front to const lane.");

        auto* newItem = new Item(m_list, std::move(value));

        InternalPushFront<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushFront<PartialLaneType>(newItem);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
    typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator 
        BypassListLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> position, const Type& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        MOLTEN_DEBUG_ASSERT(position.IsValid() == false, "Cannot insert with invalid iterator as position.");

        auto* newItem = new Item(m_list, value);

        InternalInsertNormal(position, newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalInsertPartial(position, newItem);
        }

        return Iterator(newItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
    typename BypassListLaneInterface<IsConst, TLaneType, T>::Iterator 
        BypassListLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, IsOtherReverse, TItLaneType, Type> position, Type&& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        MOLTEN_DEBUG_ASSERT(position.IsValid() == false, "Cannot insert with invalid iterator as position.");

        auto* newItem = new Item(m_list, std::move(value));

        InternalInsertNormal(position, newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalInsertPartial(position, newItem);
        }

        return Iterator(newItem);
    }


    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
    TIterator<false, IsOtherReverse, TItLaneType, T> BypassListLaneInterface<IsConst, TLaneType, T>::Erase(TIterator<IsItConst, IsOtherReverse, TItLaneType, T> it)
    {
        static_assert(!IsConst, "Cannot erase from const lane.");

        Item* item = it.m_currentItem;
        MOLTEN_DEBUG_ASSERT(item != nullptr, "Cannot erase item of from empty iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<PartialLaneType>>(*m_dataLanes).root, "Cannot erase rend iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<PartialLaneType>>(*m_dataLanes).tail, "Cannot erase end iterator.");

        auto* nextItem = BypassListLaneTraits<TItLaneType>::GetNext(item);

        if constexpr (std::is_same_v<TItLaneType, PartialLaneType> == true)
        {
            InternalErase<NormalLaneType>(item);
            InternalErase<PartialLaneType>(item);
        }
        else
        {
            InternalErase<NormalLaneType>(item);

            if (InternalIsInPartialLane(item))
            {
                InternalErase<PartialLaneType>(item);
            }
        }

        delete item;

        return TIterator<false, IsOtherReverse, TItLaneType, Type>(nextItem);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse>
    void BypassListLaneInterface<IsConst, TLaneType, T>::EnableInPartialLane(TIterator<IsItConst, IsOtherReverse, BypassListNormalLane, T> it)
    {
        static_assert(!IsConst, "Cannot enable partial lane from const lane.");
        static_assert(!std::is_same_v<TLaneType, PartialLaneType>, "Cannot enable partial lane from partial lane interface.");      

        Item* item = it.m_currentItem;
        MOLTEN_DEBUG_ASSERT(item != nullptr, "Cannot enable partial lane item of from empty iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<BypassListNormalLane>>(*m_dataLanes).root, "Cannot enable partial lane rend iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<BypassListNormalLane>>(*m_dataLanes).tail, "Cannot enable partial lane end iterator.");

        if (!InternalIsInPartialLane(item))
        {
            InternalInsertPartial(it, item);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsOtherReverse, typename TItLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::DisableInPartialLane(TIterator<IsItConst, IsOtherReverse, TItLaneType, T> it)
    {
        static_assert(!IsConst, "Cannot disable partial lane from const lane.");

        Item* item = it.m_currentItem;
        MOLTEN_DEBUG_ASSERT(item != nullptr, "Cannot enable partial lane item of from empty iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<BypassListNormalLane>>(*m_dataLanes).root, "Cannot disable partial lane rend iterator.");
        MOLTEN_DEBUG_ASSERT(item != std::get<DataLane<BypassListNormalLane>>(*m_dataLanes).tail, "Cannot disable partial lane end iterator.");

        if (InternalIsInPartialLane(item))
        {
            InternalErase<PartialLaneType>(item);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    bool BypassListLaneInterface<IsConst, TLaneType, T>::InternalIsInPartialLane(Item* item)
    {
        auto* next = BypassListLaneTraits<PartialLaneType>::GetNext(item);
        auto& laneData = std::get<DataLane<PartialLaneType>>(*m_dataLanes);

        return next != nullptr || laneData.tail == item;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::InternalPushBack(Item* item)
    {
        auto& laneData = std::get<DataLane<TProcessLaneType>>(*m_dataLanes);
        auto* endItem = laneData.tail;
        auto* preEndItem = BypassListLaneTraits<TProcessLaneType>::GetPrev(endItem);

        BypassListLaneTraits<TProcessLaneType>::SetNext(item, laneData.tail);
        BypassListLaneTraits<TProcessLaneType>::SetPrev(item, preEndItem);
        BypassListLaneTraits<TProcessLaneType>::SetPrev(laneData.tail, item);
        BypassListLaneTraits<TProcessLaneType>::SetNext(preEndItem, item);

        ++laneData.size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::InternalPushFront(Item* item)
    {
        auto& laneData = std::get<DataLane<TProcessLaneType>>(*m_dataLanes);
        auto* beginItem = laneData.root;
        auto* nextBeginItem = BypassListLaneTraits<TProcessLaneType>::GetNext(beginItem);

        BypassListLaneTraits<TProcessLaneType>::SetPrev(item, laneData.root);
        BypassListLaneTraits<TProcessLaneType>::SetNext(item, nextBeginItem);
        BypassListLaneTraits<TProcessLaneType>::SetNext(laneData.root, item);
        BypassListLaneTraits<TProcessLaneType>::SetPrev(nextBeginItem, item);

        ++laneData.size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::InternalInsertNormal(TIterator<IsItConst, IsItReverse, TItLaneType, Type> position, Item* item)
    {
        Item* positionItem = position.m_currentItem;
        auto& laneData = std::get<DataLane<NormalLaneType>>(*m_dataLanes);
        auto* prevItem = BypassListLaneTraits<NormalLaneType>::GetPrev(positionItem);

        BypassListLaneTraits<NormalLaneType>::SetNext(item, positionItem);
        BypassListLaneTraits<NormalLaneType>::SetPrev(item, prevItem);
        BypassListLaneTraits<NormalLaneType>::SetPrev(positionItem, item);
        BypassListLaneTraits<NormalLaneType>::SetNext(prevItem, item);

        ++laneData.size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, bool, typename, typename> class TIterator, bool IsItConst, bool IsItReverse, typename TItLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::InternalInsertPartial(TIterator<IsItConst, IsItReverse, TItLaneType, Type> position, Item* item)
    {
        Item* positionItem = position.m_currentItem;
        auto& laneData = std::get<DataLane<PartialLaneType>>(*m_dataLanes);
        auto* prevItem = BypassListLaneTraits<PartialLaneType>::GetPrev(positionItem);
        
        if constexpr (std::is_same_v<TItLaneType, PartialLaneType> == false)
        {
            if (!prevItem)
            {
                prevItem = InternalFindPrevPartialItem(positionItem);
            }
        }

        if (prevItem)
        {
            auto* nextItem = BypassListLaneTraits<PartialLaneType>::GetNext(prevItem);

            BypassListLaneTraits<PartialLaneType>::SetNext(item, nextItem);
            BypassListLaneTraits<PartialLaneType>::SetPrev(item, prevItem);

            BypassListLaneTraits<PartialLaneType>::SetNext(prevItem, item);
            BypassListLaneTraits<PartialLaneType>::SetPrev(nextItem, item);
        }
        else
        {
            auto* oldRoot = laneData.root;
            laneData.root = item;
            BypassListLaneTraits<PartialLaneType>::SetNext(item, oldRoot);
            BypassListLaneTraits<PartialLaneType>::SetPrev(oldRoot, item);
        }

        ++laneData.size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    typename BypassListLaneInterface<IsConst, TLaneType, T>::Item* 
        BypassListLaneInterface<IsConst, TLaneType, T>::InternalFindPrevPartialItem(Item* item)
    {
        auto* currentItem = BypassListLaneTraits<NormalLaneType>::GetPrev(item);

        while (currentItem)
        {
            auto* nextPartialItem = BypassListLaneTraits<PartialLaneType>::GetNext(currentItem);
            if (nextPartialItem)
            {
                return currentItem;
            }

            currentItem = BypassListLaneTraits<NormalLaneType>::GetPrev(currentItem);
        }

        return nullptr;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    void BypassListLaneInterface<IsConst, TLaneType, T>::InternalErase(Item* item)
    {
        auto& laneData = std::get<DataLane<TProcessLaneType>>(*m_dataLanes);
        auto* prev = BypassListLaneTraits<TProcessLaneType>::GetPrev(item);
        auto* next = BypassListLaneTraits<TProcessLaneType>::GetNext(item);

        if (laneData.root == item)
        {
            laneData.root = next;
        }
        if (laneData.tail == item)
        {
            laneData.tail = prev;
        }

        if (prev)
        {
            BypassListLaneTraits<TProcessLaneType>::SetNext(prev, next);
        }
        if (next)
        {
            BypassListLaneTraits<TProcessLaneType>::SetPrev(next, prev);
        }

        if constexpr(std::is_same_v<TProcessLaneType, PartialLaneType>)
        {
            item->prevPartial = nullptr;
            item->nextPartial = nullptr;
        }

        --laneData.size;
    }


    // Bypass list iterator interface implementations.
    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::BypassListIteratorInterface() :
        m_currentItem(nullptr)
    {}

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::BypassListIteratorInterface(Item* item) :
        m_currentItem(item)
    {}

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    template<bool IsOtherConst, typename TLaneTypeOther>
    BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::BypassListIteratorInterface(const BypassListIteratorInterface<IsOtherConst, IsReverse, TLaneTypeOther, Type>& it) :
        m_currentItem(it.m_currentItem)
    {
        static_assert(IsConst || !IsOtherConst,
            "Cannot construct from const to non-const bypass list iterator.");

        static_assert(std::is_same_v<TLaneType, TLaneTypeOther> || std::is_same_v<TLaneTypeOther, BypassListPartialLane>,
            "Cannot construct from normal to partial bypass list iterator.");
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    template<bool IsOtherConst, typename TLaneTypeOther>
    BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>&
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator = (const BypassListIteratorInterface<IsOtherConst, IsReverse, TLaneTypeOther, Type>& it)
    {
        static_assert(IsConst || !IsOtherConst,
            "Cannot assign from const to non-const bypass list iterator.");

        static_assert(std::is_same_v<TLaneType, TLaneTypeOther> || std::is_same_v<TLaneTypeOther, BypassListPartialLane>,
            "Cannot assign from normal to partial bypass list iterator.");

        m_currentItem = it.m_currentItem;
        return *this;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    bool BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::IsValid() const
    {
        return m_currentItem == nullptr;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    typename  BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::List& 
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::GetList()
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Cannot get list from iterator with nullptr item.");
        return *m_currentItem->list;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    template<bool IsConstIterator>
    std::enable_if_t<!IsConstIterator, typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Reference> 
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator *()
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Cannot dereference iterator of nullptr item.");
        MOLTEN_DEBUG_ASSERT(m_currentItem->content != nullptr, "Cannot dereference iterator of nullptr item content.");
        return m_currentItem->content->value;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    template<bool IsConstIterator>
    std::enable_if_t<IsConstIterator, typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Reference> 
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator *() const
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Cannot dereference iterator of nullptr item.");
        MOLTEN_DEBUG_ASSERT(m_currentItem->content != nullptr, "Cannot dereference iterator of nullptr item content.");
        return m_currentItem->content->value;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Iterator& 
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator ++ () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");

        if constexpr (IsReverse == false)
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        }
        else
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        }
        
        //MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Next bypass list item is nullptr.");
        return *this;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Iterator&
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator -- () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");

        if constexpr (IsReverse == false)
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        }
        else
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        }

        //MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Previous bypass list item is nullptr.");
        return *this;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Iterator
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator ++ (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        auto* previtem = m_currentItem;
        
        if constexpr (IsReverse == false)
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        }
        else
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        }

        //MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Next bypass list item is nullptr.");
        return Iterator{ previtem };
    }
    
    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    typename BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::Iterator 
        BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator -- (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        auto* prevItem = m_currentItem;
        
        if constexpr (IsReverse == false)
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        }
        else
        {
            m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        }

        //MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Previous bypass list item is nullptr.");
        return Iterator{ prevItem };
    }
    
    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    bool BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator == (const BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>& rhs) const
    {
        return m_currentItem == rhs.m_currentItem;
    }

    template<bool IsConst, bool IsReverse, typename TLaneType, typename T>
    bool BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>::operator != (const BypassListIteratorInterface<IsConst, IsReverse, TLaneType, T>& rhs) const
    {
        return m_currentItem != rhs.m_currentItem;
    }
    

    // Bypass list item implementations.
    template<typename T>
    BypassListItem<T>::BypassListItem(List* list) :
        list(list),
        content(nullptr),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    BypassListItem<T>::BypassListItem(List* list, const Type& valueCopy) :
        list(list),
        content(new Content(valueCopy)),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    BypassListItem<T>::BypassListItem(List* list, Type&& valueMove) :
        list(list),
        content(new Content(std::move(valueMove))),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    BypassListItem<T>::~BypassListItem()
    {
        delete content;
    }

    template<typename T>
    BypassListItem<T>::Content::Content(const Type& valueCopy) :
        value{ valueCopy }
    {}

    template<typename T>
    BypassListItem<T>::Content::Content(Type&& valueMove) :
        value{ std::move(valueMove) }
    {}


    // Bypass list data lane implementations.
    template<typename TLaneType, typename T>
    BypassListDataLane<TLaneType, T>::BypassListDataLane(Item* rootItem, Item* tailItem) :
        root(rootItem),
        tail(tailItem),
        size(0)
    {}

    template<typename TLaneType, typename T>
    BypassListDataLane<TLaneType, T>::BypassListDataLane(BypassListDataLane<TLaneType, T>&& lane) noexcept :
        root(lane.root),
        tail(lane.tail),
        size(lane.size)
    {
        lane.root = nullptr;
        lane.tail = nullptr;
        lane.size = 0;
    }

    template<typename TLaneType, typename T>
    BypassListDataLane<TLaneType, T>& BypassListDataLane<TLaneType, T>::operator =(BypassListDataLane<TLaneType, T>&& lane) noexcept
    {
        root = lane.root;
        lane.root = nullptr;
        tail = lane.tail;
        lane.tail = nullptr;
        size = lane.size;
        lane.size = 0;
 
        return *this;
    }


    // Bypass list lane traits implementations.
    template<>
    template<typename T>
    BypassListItem<T>* BypassListLaneTraits<BypassListNormalLane>::GetPrev(BypassListItem<T>* item)
    {
        return item->prevNormal;
    }
    template<>
    template<typename T>
    BypassListItem<T>* BypassListLaneTraits<BypassListPartialLane>::GetPrev(BypassListItem<T>* item)
    {
        return item->prevPartial;
    }

    template<>
    template<typename T>
    BypassListItem<T>* BypassListLaneTraits<BypassListNormalLane>::GetNext(BypassListItem<T>* item)
    {
        return item->nextNormal;
    }
    template<>
    template<typename T>
    BypassListItem<T>* BypassListLaneTraits<BypassListPartialLane>::GetNext(BypassListItem<T>* item)
    {
        return item->nextPartial;
    }

    template<>
    template<typename T>
    void BypassListLaneTraits<BypassListNormalLane>::SetPrev(BypassListItem<T>* item, BypassListItem<T>* prev)
    {
        item->prevNormal = prev;
    }
    template<>
    template<typename T>
    void BypassListLaneTraits<BypassListPartialLane>::SetPrev(BypassListItem<T>* item, BypassListItem<T>* prev)
    {
        item->prevPartial = prev;
    }

    template<>
    template<typename T>
    void BypassListLaneTraits<BypassListNormalLane>::SetNext(BypassListItem<T>* item, BypassListItem<T>* next)
    {
        item->nextNormal = next;
    }
    template<>
    template<typename T>
    void BypassListLaneTraits<BypassListPartialLane>::SetNext(BypassListItem<T>* item, BypassListItem<T>* next)
    {
        item->nextPartial = next;
    }

}
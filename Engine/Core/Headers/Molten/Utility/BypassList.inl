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
    inline BypassList<T>::BypassList() :
        m_endItem(new Item()),
        m_dataLanes(std::make_tuple(m_endItem, m_endItem))
    {}

    template<typename T>
    inline BypassList<T>::~BypassList()
    {
        InternalClearIncludingEndItem();
    }

    template<typename T>
    inline BypassList<T>::BypassList(BypassList&& list) noexcept :
        m_endItem(list.m_endItem),
        m_dataLanes(std::move(list.m_dataLanes))
    {
        list.m_endItem = nullptr;
    }

    template<typename T>
    inline BypassList<T>& BypassList<T>::operator =(BypassList&& list) noexcept
    {
        InternalClearIncludingEndItem();

        m_dataLanes = std::move(list.m_dataLanes);
        m_endItem = list.m_endItem;
        list.m_endItem = nullptr;
        return *this;
    }

    template<typename T>
    template<typename TLaneType>
    inline typename BypassList<T>::template Lane<TLaneType> BypassList<T>::GetLane()
    {
        return Lane<TLaneType>(&m_dataLanes);
    }
    template<typename T>
    template<typename TLaneType>
    inline typename BypassList<T>::template ConstLane<TLaneType> BypassList<T>::GetLane() const
    {
        return ConstLane<TLaneType>(&m_dataLanes);
    }

    template<typename T>
    inline size_t BypassList<T>::GetSize() const
    {
        return std::get<DataLane<NormalLaneType>>(m_dataLanes).size;
    }

    template<typename T>
    inline void BypassList<T>::Clear()
    {
        auto& dataLane = std::get<DataLane<NormalLaneType>>(m_dataLanes);
        auto* currentItem = dataLane.root;
        while (currentItem != m_endItem)
        {
            auto* prevNode = currentItem;
            currentItem = BypassListLaneTraits<NormalLaneType>::GetNext(prevNode);
            delete prevNode;
        }
    }

    template<typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    TIterator<false, TItLaneType, T> BypassList<T>::Erase(TIterator<IsItConst, TItLaneType, Type> it)
    {
        return GetLane<TItLaneType>().Erase(it);
    }

    template<typename T>
    inline void BypassList<T>::InternalClearIncludingEndItem()
    {
        Clear();
        delete m_endItem;
        m_endItem = nullptr;
    }

 
    // Bypass list lane interface implementations.
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListLaneInterface<IsConst, TLaneType, T>::BypassListLaneInterface() :
        m_dataLanes(nullptr)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListLaneInterface<IsConst, TLaneType, T>::BypassListLaneInterface(DataLanes* dataLanes) :
        m_dataLanes(dataLanes)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassListLaneInterface<IsConst, TLaneType, T>::IsEmpty() const
    {
        return m_dataLanes == nullptr;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline size_t BypassListLaneInterface<IsConst, TLaneType, T>::GetSize() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get size from empty bypass list lane.");
        return std::get<DataLane<TLaneType>>(*m_dataLanes).size;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t < !IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template ConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin iterator from empty bypass list lane.");
        return ConstIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::begin() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get begin const iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).root);
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::end()
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<!IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template ConstIterator> BypassListLaneInterface<IsConst, TLaneType, T>::end() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end iterator from empty bypass list lane.");
        return ConstIterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstLane>
    inline std::enable_if_t<IsConstLane, typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator> BypassListLaneInterface<IsConst, TLaneType, T>::end() const
    {
        MOLTEN_DEBUG_ASSERT(m_dataLanes != nullptr, "Cannot get end const iterator from empty bypass list lane.");
        return Iterator(std::get<DataLane<TLaneType>>(*m_dataLanes).tail);
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::PushBack(const Type& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(value);

        InternalPushBack<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushBack<PartialLaneType>(newItem);
        }
    }
    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::PushBack(Type&& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(std::move(value));

        InternalPushBack<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushBack<PartialLaneType>(newItem);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::PushFront(const Type& value)
    {
        static_assert(!IsConst, "Cannot push back to const lane.");

        auto* newItem = new Item(value);

        InternalPushFront<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushFront<PartialLaneType>(newItem);
        }
    }
    template<bool IsConst, typename TLaneType, typename T>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::PushFront(Type&& value)
    {
        static_assert(!IsConst, "Cannot push front to const lane.");

        auto* newItem = new Item(std::move(value));

        InternalPushFront<NormalLaneType>(newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalPushFront<PartialLaneType>(newItem);
        }
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator 
        BypassListLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, const Type& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        MOLTEN_DEBUG_ASSERT(position.IsEmpty() == false, "Cannot insert with empty iterator as position.");

        auto* newItem = new Item(value);

        InternalInsertNormal(position, newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalInsertPartial(position, newItem);
        }

        return Iterator(newItem);
    }
    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline typename BypassListLaneInterface<IsConst, TLaneType, T>::template Iterator 
        BypassListLaneInterface<IsConst, TLaneType, T>::Insert(TIterator<IsItConst, TItLaneType, Type> position, Type&& value)
    {
        static_assert(!IsConst, "Cannot insert to const lane.");

        MOLTEN_DEBUG_ASSERT(position.IsEmpty() == false, "Cannot insert with empty iterator as position.");

        auto* newItem = new Item(std::move(value));

        InternalInsertNormal(position, newItem);

        if constexpr (std::is_same_v<TLaneType, PartialLaneType> == true)
        {
            InternalInsertPartial(position, newItem);
        }

        return Iterator(newItem);
    }


    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline TIterator<false, TItLaneType, T> BypassListLaneInterface<IsConst, TLaneType, T>::Erase(TIterator<IsItConst, TItLaneType, T> it)
    {
        static_assert(!IsConst, "Cannot erase from const lane.");

        Item* item = it.m_currentItem;
        MOLTEN_DEBUG_ASSERT(item != nullptr, "Cannot erase item of from empty iterator.");
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

        return TIterator<false, TItLaneType, Type>(nextItem);
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassListLaneInterface<IsConst, TLaneType, T>::InternalIsInPartialLane(Item* item)
    {
        auto* next = BypassListLaneTraits<PartialLaneType>::GetNext(item);
        auto& laneData = std::get<DataLane<PartialLaneType>>(*m_dataLanes);

        return next != nullptr || laneData.tail == item;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::InternalPushBack(Item* item)
    {
        auto& laneData = std::get<DataLane<TProcessLaneType>>(*m_dataLanes);
        auto* endItem = laneData.tail;
        auto* preTail = BypassListLaneTraits<TProcessLaneType>::GetPrev(laneData.tail);

        BypassListLaneTraits<TProcessLaneType>::SetPrev(laneData.tail, item);
        BypassListLaneTraits<TProcessLaneType>::SetNext(item, laneData.tail);
        BypassListLaneTraits<TProcessLaneType>::SetPrev(item, preTail);

        if (preTail)
        {
            BypassListLaneTraits<TProcessLaneType>::SetNext(preTail, item);
        }

        if (laneData.root == endItem)
        {
            laneData.root = item;
        }

        laneData.size++;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::InternalPushFront(Item* item)
    {
        auto& laneData = std::get<DataLane<TProcessLaneType>>(*m_dataLanes);

        BypassListLaneTraits<TProcessLaneType>::SetNext(item, laneData.root);
        BypassListLaneTraits<TProcessLaneType>::SetPrev(laneData.root, item);
        laneData.root = item;

        laneData.size++;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::InternalInsertNormal(TIterator<IsItConst, TItLaneType, Type> position, Item* item)
    {
        Item* positionItem = position.m_currentItem;
        auto& laneData = std::get<DataLane<NormalLaneType>>(*m_dataLanes);
        auto* prevItem = BypassListLaneTraits<NormalLaneType>::GetPrev(positionItem);
        
        if (prevItem)
        {
            BypassListLaneTraits<NormalLaneType>::SetNext(prevItem, item);
            BypassListLaneTraits<NormalLaneType>::SetPrev(item, prevItem);
        }
        else
        {
            laneData.root = item;
        }

        BypassListLaneTraits<NormalLaneType>::SetNext(item, positionItem);
        BypassListLaneTraits<NormalLaneType>::SetPrev(positionItem, item);

        laneData.size++;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<template <bool, typename, typename> class TIterator, bool IsItConst, typename TItLaneType>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::InternalInsertPartial(TIterator<IsItConst, TItLaneType, Type> position, Item* item)
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

        laneData.size++;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassListLaneInterface<IsConst, TLaneType, T>::template Item* 
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
        };

        return nullptr;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<typename TProcessLaneType>
    inline void BypassListLaneInterface<IsConst, TLaneType, T>::InternalErase(Item* item)
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

        laneData.size--;
    }


    // Bypass list iterator interface implementations.
    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListIteratorInterface<IsConst, TLaneType, T>::BypassListIteratorInterface() :
        m_currentItem(nullptr)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListIteratorInterface<IsConst, TLaneType, T>::BypassListIteratorInterface(Item* item) :
        m_currentItem(item)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListIteratorInterface<IsConst, TLaneType, T>::BypassListIteratorInterface(const BypassListIteratorInterface<IsConst, BypassListPartialLane, Type>& it) :
        m_currentItem(it.m_currentItem)
    {}

    template<bool IsConst, typename TLaneType, typename T>
    inline BypassListIteratorInterface<IsConst, TLaneType, T>& BypassListIteratorInterface<IsConst, TLaneType, T>::operator =(const BypassListIteratorInterface<IsConst, BypassListPartialLane, Type>& it)
    {
        m_currentItem = it.m_currentItem;
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassListIteratorInterface<IsConst, TLaneType, T>::IsEmpty() const
    {
        return m_currentItem == nullptr;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstIterator>
    inline std::enable_if_t<!IsConstIterator, typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Reference> BypassListIteratorInterface<IsConst, TLaneType, T>::operator *()
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Cannot dereference iterator of nullptr item.");
        MOLTEN_DEBUG_ASSERT(m_currentItem->content != nullptr, "Cannot dereference iterator of nullptr item content.");
        return m_currentItem->content->value;
    }

    template<bool IsConst, typename TLaneType, typename T>
    template<bool IsConstIterator>
    inline std::enable_if_t<IsConstIterator, typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Reference> BypassListIteratorInterface<IsConst, TLaneType, T>::operator *() const
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Cannot dereference iterator of nullptr item.");
        MOLTEN_DEBUG_ASSERT(m_currentItem->content != nullptr, "Cannot dereference iterator of nullptr item content.");
        return m_currentItem->content->value;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Iterator& BypassListIteratorInterface<IsConst, TLaneType, T>::operator ++ () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Next bypass list item is nullptr.");
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Iterator& BypassListIteratorInterface<IsConst, TLaneType, T>::operator -- () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Previous bypass list item is nullptr.");
        return *this;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Iterator BypassListIteratorInterface<IsConst, TLaneType, T>::operator ++ (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        auto* previtem = m_currentItem;
        m_currentItem = BypassListLaneTraits<TLaneType>::template GetNext<T>(m_currentItem);
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Next bypass list item is nullptr.");
        return Iterator{ previtem };
    }
    
    template<bool IsConst, typename TLaneType, typename T>
    inline typename BypassListIteratorInterface<IsConst, TLaneType, T>::template Iterator BypassListIteratorInterface<IsConst, TLaneType, T>::operator -- (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Current bypass list item is nullptr.");
        auto* prevItem = m_currentItem;
        m_currentItem = BypassListLaneTraits<TLaneType>::template GetPrev<T>(m_currentItem);
        MOLTEN_DEBUG_ASSERT(m_currentItem != nullptr, "Previous bypass list item is nullptr.");
        return Iterator{ prevItem };
    }
    
    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassListIteratorInterface<IsConst, TLaneType, T>::operator == (const BypassListIteratorInterface<IsConst, TLaneType, T>& rhs) const
    {
        return m_currentItem == rhs.m_currentItem;
    }

    template<bool IsConst, typename TLaneType, typename T>
    inline bool BypassListIteratorInterface<IsConst, TLaneType, T>::operator != (const BypassListIteratorInterface<IsConst, TLaneType, T>& rhs) const
    {
        return m_currentItem != rhs.m_currentItem;
    }
    

    // Bypass list item implementations.
    template<typename T>
    inline BypassListItem<T>::BypassListItem() :
        content(nullptr),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    inline BypassListItem<T>::BypassListItem(const Type& valueCopy) :
        content(new Content(valueCopy)),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    inline BypassListItem<T>::BypassListItem(Type&& valueMove) :
        content(new Content(std::move(valueMove))),
        prevNormal(nullptr),
        prevPartial(nullptr),
        nextNormal(nullptr),
        nextPartial(nullptr)
    {}

    template<typename T>
    inline BypassListItem<T>::~BypassListItem()
    {
        delete content;
    }

    template<typename T>
    inline BypassListItem<T>::Content::Content(const Type& valueCopy) :
        value{ valueCopy }
    {}

    template<typename T>
    inline BypassListItem<T>::Content::Content(Type&& valueMove) :
        value{ std::move(valueMove) }
    {}


    // Bypass list data lane implementations.
    template<typename TLaneType, typename T>
    inline BypassListDataLane<TLaneType, T>::BypassListDataLane(Item* endItem) :
        root(endItem),
        tail(endItem),
        size(0)
    {}

    template<typename TLaneType, typename T>
    inline BypassListDataLane<TLaneType, T>::BypassListDataLane(BypassListDataLane<TLaneType, T>&& lane) noexcept :
        root(lane.root),
        tail(lane.tail),
        size(lane.size)
    {
        lane.root = nullptr;
        lane.tail = nullptr;
        lane.size = 0;
    }

    template<typename TLaneType, typename T>
    inline BypassListDataLane<TLaneType, T>& BypassListDataLane<TLaneType, T>::operator =(BypassListDataLane<TLaneType, T>&& lane) noexcept
    {
        root = lane.root;
        tail = lane.tail;
        size = lane.size;
        lane.root = nullptr;
        lane.tail = nullptr;
        lane.size = 0;
        return *this;
    }


    // Bypass list lane traits implementations.
    template<>
    template<typename T>
    inline BypassListItem<T>* BypassListLaneTraits<BypassListNormalLane>::GetPrev(BypassListItem<T>* item)
    {
        return item->prevNormal;
    }
    template<>
    template<typename T>
    inline BypassListItem<T>* BypassListLaneTraits<BypassListPartialLane>::GetPrev(BypassListItem<T>* item)
    {
        return item->prevPartial;
    }

    template<>
    template<typename T>
    inline BypassListItem<T>* BypassListLaneTraits<BypassListNormalLane>::GetNext(BypassListItem<T>* item)
    {
        return item->nextNormal;
    }
    template<>
    template<typename T>
    inline BypassListItem<T>* BypassListLaneTraits<BypassListPartialLane>::GetNext(BypassListItem<T>* item)
    {
        return item->nextPartial;
    }

    template<>
    template<typename T>
    inline void BypassListLaneTraits<BypassListNormalLane>::SetPrev(BypassListItem<T>* item, BypassListItem<T>* prev)
    {
        item->prevNormal = prev;
    }
    template<>
    template<typename T>
    inline void BypassListLaneTraits<BypassListPartialLane>::SetPrev(BypassListItem<T>* item, BypassListItem<T>* prev)
    {
        item->prevPartial = prev;
    }

    template<>
    template<typename T>
    inline void BypassListLaneTraits<BypassListNormalLane>::SetNext(BypassListItem<T>* item, BypassListItem<T>* next)
    {
        item->nextNormal = next;
    }
    template<>
    template<typename T>
    inline void BypassListLaneTraits<BypassListPartialLane>::SetNext(BypassListItem<T>* item, BypassListItem<T>* next)
    {
        item->nextPartial = next;
    }

}
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

    // Alterante list iterator implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::Iterator<TPathType>::Iterator() :
        m_currentNode(nullptr)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::Iterator<TPathType>::Iterator(AlternateListNode<T>* currentNode) :
        m_currentNode(currentNode)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::Iterator<TPathType>::IsEmpty() const
    {
        return m_currentNode == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::Type& AlternateList<T>::Iterator<TPathType>::operator *() const
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Cannot dereference iterator of nullptr node.");
        MOLTEN_DEBUG_ASSERT(m_currentNode->content != nullptr, "Cannot dereference end iterator.");
        return m_currentNode->content->value;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType>& AlternateList<T>::Iterator<TPathType>::operator ++ () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Next alternate list node is nullptr.");
        return *this;
    }
    
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType>& AlternateList<T>::Iterator<TPathType>::operator -- () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Previous alternate list node is nullptr.");
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::Iterator<TPathType>::operator ++ (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Next alternate list node is nullptr.");
        return Iterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::Iterator<TPathType>::operator -- (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Previous alternate list node is nullptr.");
        return Iterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::Iterator<TPathType>::operator == (const Iterator<TPathType>& rhs) const
    {
        return m_currentNode == rhs.m_currentNode;
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::Iterator<TPathType>::operator != (const Iterator<TPathType> & rhs) const
    {
        return m_currentNode != rhs.m_currentNode;
    }


    // Alterante const list iterator implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIterator<TPathType>::ConstIterator() :
        m_currentNode(nullptr)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIterator<TPathType>::ConstIterator(const AlternateListNode<T>* currentNode) :
        m_currentNode(currentNode)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::ConstIterator<TPathType>::IsEmpty() const
    {
        return m_currentNode == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline const typename AlternateList<T>::Type& AlternateList<T>::ConstIterator<TPathType>::operator *() const
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Cannot dereference iterator of nullptr node.");
        return m_currentNode->value;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType>& AlternateList<T>::ConstIterator<TPathType>::operator ++ () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Next alternate list node is nullptr.");
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType>& AlternateList<T>::ConstIterator<TPathType>::operator -- () // Pre
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Previous alternate list node is nullptr.");     
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIterator<TPathType>::operator ++ (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Next alternate list node is nullptr.");
        return ConstIterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIterator<TPathType>::operator -- (int) // Post
    {
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Current alternate list node is nullptr.");
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        MOLTEN_DEBUG_ASSERT(m_currentNode != nullptr, "Previous alternate list node is nullptr.");
        return ConstIterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::ConstIterator<TPathType>::operator == (const ConstIterator<TPathType>& rhs) const
    {
        return m_currentNode == rhs.m_currentNode;
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::ConstIterator<TPathType>::operator != (const ConstIterator<TPathType>& rhs) const
    {
        return m_currentNode != rhs.m_currentNode;
    }


    // Alterante list iterator path implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::IteratorPath<TPathType>::IteratorPath() :
        m_list(nullptr)
    { }

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::IteratorPath<TPathType>::IteratorPath(AlternateList<Type>* list) :
        m_list(list)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::IteratorPath<TPathType>::IsEmpty() const
    {
        return m_list == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateList<T>::IteratorPath<TPathType>::GetSize() const
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get size of empty iterator path.");
        return m_list->GetSize<TPathType>();
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::begin()
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get begin iterator of empty iterator path.");
        return Iterator<TPathType>{ std::get<AlternateListPath<Type, TPathType>>(m_list->m_paths).root };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::end()
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get end iterator of empty iterator path.");
        return Iterator<TPathType>{ std::get<AlternateListPath<Type, TPathType>>(m_list->m_paths).tail };
    }


    // Alterante list const iterator path implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIteratorPath<TPathType>::ConstIteratorPath() :
        m_list(nullptr)
    { }

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIteratorPath<TPathType>::ConstIteratorPath(const AlternateList<Type>* list) :
        m_list(list)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::ConstIteratorPath<TPathType>::IsEmpty() const
    {
        return m_list == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateList<T>::ConstIteratorPath<TPathType>::GetSize() const
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get size of empty const iterator path.");
        return m_list->GetSize<TPathType>();
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIteratorPath<TPathType>::begin()
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get begin iterator of empty const iterator path.");
        return ConstIterator<TPathType>{ std::get<AlternateListPath<Type, TPathType>>(m_list->m_paths).root };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIteratorPath<TPathType>::end()
    {
        MOLTEN_DEBUG_ASSERT(IsEmpty() == false, "Cannot get end iterator of empty const iterator path.");
        return ConstIterator<TPathType>{ std::get<AlternateListPath<Type, TPathType>>(m_list->m_paths).tail };
    }


    // Alterante list implementations.
    template<typename T>
    inline AlternateList<T>::AlternateList() :
        m_endNode(new AlternateListNode<T>()),
        m_paths(std::make_tuple(m_endNode, m_endNode))
    {}

    template<typename T>
    inline AlternateList<T>::~AlternateList()
    {
        InternalDeleteAllNodes();
    }

    template<typename T>
    inline AlternateList<T>::AlternateList(AlternateList&& list) noexcept :
        m_endNode(list.m_endNode),
        m_paths(std::move(list.m_paths))
    {
        list.m_endNode = nullptr;
    }

    template<typename T>
    inline AlternateList<T>& AlternateList<T>::operator =(AlternateList&& list) noexcept
    {
        InternalDeleteAllNodes();

        m_paths = std::move(list.m_paths);
        list.m_endNode = nullptr;
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateList<T>::GetSize() const
    {  
        return std::get<AlternateListPath<Type, TPathType>>(m_paths).size;
    }

    template<typename T>
    inline size_t AlternateList<T>::GetMainSize() const
    {
        return std::get<AlternateListPath<Type, MainPath>>(m_paths).size;
    }

    template<typename T>
    inline size_t AlternateList<T>::GetSubSize() const
    {
        return std::get<AlternateListPath<Type, SubPath>>(m_paths).size;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template IteratorPath<TPathType> AlternateList<T>::GetPath()
    {
        return IteratorPath<TPathType>{ this };
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIteratorPath<TPathType> AlternateList<T>::GetPath() const
    {
        return ConstIteratorPath<TPathType>{ this };
    }

    template<typename T>
    inline typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::MainPath> AlternateList<T>::GetMainPath()
    {
        return IteratorPath<MainPath>{ this };
    }
    template<typename T>
    inline typename AlternateList<T>::template ConstIteratorPath<typename AlternateList<T>::MainPath> AlternateList<T>::GetMainPath() const
    {
        return ConstIteratorPath<MainPath>{ this };
    }

    template<typename T>
    inline typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::SubPath> AlternateList<T>::GetSubPath()
    {
        return IteratorPath<SubPath>{ this };
    }
    template<typename T>
    inline typename AlternateList<T>::template ConstIteratorPath<typename AlternateList<T>::SubPath> AlternateList<T>::GetSubPath() const
    {
        return ConstIteratorPath<SubPath>{ this };
    }

    template<typename T>
    inline void AlternateList<T>::PushBack(const bool addSubPath, const Type& value)
    {
        auto newNode = new AlternateListNode<T>(value);

        InternalPushBack<MainPath>(newNode);

        if (addSubPath)
        {
            InternalPushBack<SubPath>(newNode);
        }
    }

    template<typename T>
    inline void AlternateList<T>::PushBack(const bool addSubPath, Type&& value)
    {
        auto newNode = new AlternateListNode<T>(std::move(value));

        InternalPushBack<MainPath>(newNode);

        if (addSubPath)
        {
            InternalPushBack<SubPath>(newNode);
        }
    }

    template<typename T>
    inline void AlternateList<T>::PushFront(const bool addSubPath, const Type& value)
    {
        auto newNode = new AlternateListNode<T>(value);

        InternalPushFront<MainPath>(newNode);

        if (addSubPath)
        {
            InternalPushFront<SubPath>(newNode);
        }
    }

    template<typename T>
    inline void AlternateList<T>::PushFront(const bool addSubPath, Type&& value)
    {
        auto newNode = new AlternateListNode<T>(std::move(value));

        InternalPushFront<MainPath>(newNode);

        if (addSubPath)
        {
            InternalPushFront<SubPath>(newNode);
        }
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<AlternateListMainPath> AlternateList<T>::Insert(Iterator<TPathType> position, const bool addSubPath, const Type& value)
    {
        MOLTEN_DEBUG_ASSERT(position.IsEmpty() == false, "Cannot insert with empty iterator as position.");

        auto newNode = new AlternateListNode<T>(value);

        InternalInsertMain(position, newNode);

        if (addSubPath)
        {
            InternalInsertSub(position, newNode);
        }

        return Iterator<MainPath>(newNode);
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<AlternateListMainPath> AlternateList<T>::Insert(Iterator<TPathType> position, const bool addSubPath, Type&& value)
    {
        MOLTEN_DEBUG_ASSERT(position.IsEmpty() == false, "Cannot insert with empty iterator as position.");

        auto newNode = new AlternateListNode<T>(std::move(value));

        InternalInsertMain(position, newNode);

        if (addSubPath)
        {
            InternalInsertSub(position, newNode);
        }

        return Iterator<MainPath>(newNode);
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::Erase(Iterator<TPathType> it)
    {
        auto* node = it.m_currentNode;
        if (node == m_endNode)
        {
            MOLTEN_DEBUG_ASSERT(node != m_endNode, "Erasing end element.");
            return Iterator<TPathType>{ m_endNode };
        }
        auto* nextPathNode = AlternateListPathTraits<TPathType>::GetNext(node);
        
        InternalErase<MainPath>(node);

        if (InternalIsInSubPath(node))
        {
            InternalErase<SubPath>(node);
        }

        delete node;
        return Iterator<TPathType>{ nextPathNode };
    }

    template<typename T>
    bool AlternateList<T>::InternalIsInSubPath(AlternateListNode<T>* node)
    {
        auto& path = std::get<AlternateListPath<Type, SubPath>>(m_paths);
        auto* prev = AlternateListPathTraits<SubPath>::GetPrev(node);
        auto* next = AlternateListPathTraits<SubPath>::GetNext(node);

        return
            path.root == node || path.tail == node ||
            (prev && AlternateListPathTraits<SubPath>::GetNext(prev) == node) ||
            (next && AlternateListPathTraits<SubPath>::GetPrev(next) == node);
    }

    template<typename T>
    void AlternateList<T>::InternalDeleteAllNodes()
    {
        auto& path = std::get<AlternateListPath<Type, MainPath>>(m_paths);
        auto currentNode = path.root;
        while (currentNode)
        {
            auto prevNode = currentNode;
            currentNode = AlternateListPathTraits<MainPath>::GetNext(prevNode);
            delete prevNode;
        }
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalPushBack(AlternateListNode<T>* node)
    {
        auto& path = std::get<AlternateListPath<Type, TPathType>>(m_paths);
        auto preTail = AlternateListPathTraits<TPathType>::GetPrev(path.tail);

        AlternateListPathTraits<TPathType>::SetPrev(path.tail, node);
        AlternateListPathTraits<TPathType>::SetNext(node, path.tail);
        AlternateListPathTraits<TPathType>::SetPrev(node, preTail);

        if (preTail)
        {
            AlternateListPathTraits<TPathType>::SetNext(preTail, node);
        }

        if (path.root == m_endNode)
        {
            path.root = node;
        }

        path.size++;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalPushFront(AlternateListNode<T>* node)
    {
        auto& path = std::get<AlternateListPath<Type, TPathType>>(m_paths);

        AlternateListPathTraits<TPathType>::SetNext(node, path.root);
        AlternateListPathTraits<TPathType>::SetPrev(path.root, node);
        path.root = node;

        path.size++;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalInsertMain(Iterator<TPathType> position, AlternateListNode<T>* node)
    {
        auto positionNode = position.m_currentNode;
        auto prevNode = AlternateListPathTraits<MainPath>::GetPrev(positionNode);
        auto& path = std::get<AlternateListPath<Type, MainPath>>(m_paths);

        if (prevNode)
        {
            AlternateListPathTraits<MainPath>::SetNext(prevNode, node);
            AlternateListPathTraits<MainPath>::SetPrev(node, prevNode);
        }
        else
        {
            path.root = node;
        }

        AlternateListPathTraits<MainPath>::SetNext(node, positionNode);
        AlternateListPathTraits<MainPath>::SetPrev(positionNode, node);
        
        path.size++;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalInsertSub(Iterator<TPathType> position, AlternateListNode<T>* node)
    {
        auto positionNode = position.m_currentNode;
        auto& path = std::get<AlternateListPath<Type, SubPath>>(m_paths);

        auto prevSub = AlternateListPathTraits<SubPath>::GetPrev(positionNode);
        if (!prevSub)
        {
            prevSub = InternalFindPrevSubNode(positionNode);
        }

        if (prevSub)
        {
            auto nextSub = AlternateListPathTraits<SubPath>::GetNext(prevSub);

            AlternateListPathTraits<SubPath>::SetNext(node, nextSub);
            AlternateListPathTraits<SubPath>::SetPrev(node, prevSub);

            AlternateListPathTraits<SubPath>::SetNext(prevSub, node);
            AlternateListPathTraits<SubPath>::SetPrev(nextSub, node);
        }
        else
        {
            auto oldRoot = path.root;
            path.root = node;
            AlternateListPathTraits<SubPath>::SetNext(node, oldRoot);
            AlternateListPathTraits<SubPath>::SetPrev(oldRoot, node);
        }

        path.size++;
    }

    template<typename T>
    inline AlternateListNode<T>* AlternateList<T>::InternalFindPrevSubNode(AlternateListNode<T>* node)
    {
        auto currentNode = AlternateListPathTraits<MainPath>::GetPrev(node);
        while(currentNode)
        {   
            auto nextSub = AlternateListPathTraits<SubPath>::GetNext(currentNode);
            if (nextSub)
            {
                return currentNode;
            }

            currentNode = AlternateListPathTraits<MainPath>::GetPrev(currentNode);
        };

        return nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalErase(AlternateListNode<T>* node)
    {
       // Remove from main path.
       auto& path = std::get<AlternateListPath<Type, TPathType>>(m_paths);
       auto* prev = AlternateListPathTraits<TPathType>::GetPrev(node);
       auto* next = AlternateListPathTraits<TPathType>::GetNext(node);

       if (path.root == node)
       {
           path.root = next;
       }
       if (path.tail == node)
       {
           path.tail = prev;
       }

       if (prev)
       {
           AlternateListPathTraits<TPathType>::SetNext(prev, next);
       }
       if (next)
       {
           AlternateListPathTraits<TPathType>::SetPrev(next, prev);
       }
       path.size--;
    }


    // Alterante list node implementations.
    template<typename T>
    inline AlternateListNode<T>::AlternateListNode() :
        content(nullptr),
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}

    template<typename T>
    inline AlternateListNode<T>::AlternateListNode(const Type& valueCopy) :
        content(new Content(valueCopy)),
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}

    template<typename T>
    inline AlternateListNode<T>::AlternateListNode(Type&& valueMove) :
        content(new Content(std::move(valueMove))),
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}

    template<typename T>
    inline AlternateListNode<T>::~AlternateListNode()
    {
        delete content;
    }

    template<typename T>
    inline AlternateListNode<T>::Content::Content(const Type& valueCopy) :
        value{ valueCopy }
    {}

    template<typename T>
    inline AlternateListNode<T>::Content::Content(Type&& valueMove) :
        value{ std::move(valueMove) }
    {}


    // Alternate list path implementations.
    template<typename T, typename TPathType>
    inline AlternateListPath<T, TPathType>::AlternateListPath(AlternateListNode<T>* end) :
        root(end),
        tail(end),
        size(0)
    {}

    template<typename T, typename TPathType>
    inline AlternateListPath<T, TPathType>::AlternateListPath(AlternateListPath<T, TPathType>&& path) noexcept :
        root(path.root),
        tail(path.tail),
        size(path.size)
    {
        path.root = nullptr;
        path.tail = nullptr;
        path.size = 0;
    }

    template<typename T, typename TPathType>
    inline AlternateListPath<T, TPathType>& AlternateListPath<T, TPathType>::operator =(AlternateListPath<T, TPathType>&& path) noexcept
    {
        root = path.root;
        tail = path.tail;
        size = path.size;
        path.root = nullptr;
        path.tail = nullptr;
        path.size = 0;
        return *this;
    }


    // Alterante list path traits implementations.
    template<>
    template<typename T>
    inline AlternateListNode<T>* AlternateListPathTraits<AlternateListMainPath>::GetPrev(AlternateListNode<T>* node)
    {
        return node->prevMain;
    }
    template<>
    template<typename T>
    inline AlternateListNode<T>* AlternateListPathTraits<AlternateListSubPath>::GetPrev(AlternateListNode<T>* node)
    {
        return node->prevSub;
    }

    template<>
    template<typename T>
    inline AlternateListNode<T>* AlternateListPathTraits<AlternateListMainPath>::GetNext(AlternateListNode<T>* node)
    {
        return node->nextMain;
    }
    template<>
    template<typename T>
    inline AlternateListNode<T>* AlternateListPathTraits<AlternateListSubPath>::GetNext(AlternateListNode<T>* node)
    {
        return node->nextSub;
    }

    template<>
    template<typename T>
    inline void AlternateListPathTraits<AlternateListMainPath>::SetPrev(AlternateListNode<T>* node, AlternateListNode<T>* prev)
    {
        node->prevMain = prev;
    }
    template<>
    template<typename T>
    inline void AlternateListPathTraits<AlternateListSubPath>::SetPrev(AlternateListNode<T>* node, AlternateListNode<T>* prev)
    {
        node->prevSub = prev;
    }

    template<>
    template<typename T>
    inline void AlternateListPathTraits<AlternateListMainPath>::SetNext(AlternateListNode<T>* node, AlternateListNode<T>* next)
    {
        node->nextMain = next;
    }
    template<>
    template<typename T>
    inline void AlternateListPathTraits<AlternateListSubPath>::SetNext(AlternateListNode<T>* node, AlternateListNode<T>* next)
    {
        node->nextSub = next;
    }

}
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

    // Alterante iterator implementations.
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
    inline typename AlternateList<T>::Type& AlternateList<T>::Iterator<TPathType>::operator *() const
    {
        return m_currentNode->value;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType>& AlternateList<T>::Iterator<TPathType>::operator ++ () // Pre
    {
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        return *this;
    }
    
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType>& AlternateList<T>::Iterator<TPathType>::operator -- () // Pre
    {
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::Iterator<TPathType>::operator ++ (int) // Post
    {
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        return Iterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::Iterator<TPathType>::operator -- (int) // Post
    {
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        return Iterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::template Iterator<TPathType>::operator == (const Iterator<TPathType>& rhs) const
    {
        return m_currentNode == rhs.m_currentNode;
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateList<T>::Iterator<TPathType>::operator != (const Iterator<TPathType> & rhs) const
    {
        return m_currentNode != rhs.m_currentNode;
    }


    // Alterante const iterator implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIterator<TPathType>::ConstIterator() :
        m_currentNode(nullptr)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::ConstIterator<TPathType>::ConstIterator(AlternateListNode<T>* currentNode) :
        m_currentNode(currentNode)
    {}

    template<typename T>
    template<typename TPathType>
    inline const typename AlternateList<T>::Type& AlternateList<T>::ConstIterator<TPathType>::operator *() const
    {
        return m_currentNode->value;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType>& AlternateList<T>::ConstIterator<TPathType>::operator ++ () // Pre
    {
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType>& AlternateList<T>::ConstIterator<TPathType>::operator -- () // Pre
    {
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIterator<TPathType>::operator ++ (int) // Post
    {
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetNext<T>(m_currentNode);
        return ConstIterator{ prevCurrent };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::ConstIterator<TPathType>::operator -- (int) // Post
    {
        auto* prevCurrent = m_currentNode;
        m_currentNode = AlternateListPathTraits<TPathType>::template GetPrev<T>(m_currentNode);
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


    // Alterante iterator path implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::IteratorPath<TPathType>::IteratorPath() :
        m_beginNode(nullptr)
    { }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::begin()
    {
        return Iterator<TPathType>{ m_beginNode };
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::begin() const
    {
        return ConstIterator<TPathType>{ m_beginNode };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template Iterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::end()
    {
        return Iterator<TPathType>{ nullptr };
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template ConstIterator<TPathType> AlternateList<T>::IteratorPath<TPathType>::end() const
    {
        return ConstIterator<TPathType>{ nullptr };
    }

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::IteratorPath<TPathType>::IteratorPath(AlternateListNode<T>* beginNode) :
        m_beginNode(beginNode)
    {}


    // Alterante list implementations.
    template<typename T>
    inline AlternateList<T>::~AlternateList()
    {
        auto& path = std::get<Path<MainPath>>(m_paths);
        auto currentNode = path.root;
        while (currentNode)
        {
            auto prevNode = currentNode;
            currentNode = AlternateListPathTraits<MainPath>::GetNext(prevNode);
            delete prevNode;
        }
    }

    template<typename T>
    inline AlternateList<T>::AlternateList(AlternateList&& list) :
        m_paths(std::move(list.m_paths))
    {}

    template<typename T>
    inline AlternateList<T>& AlternateList<T>::operator =(AlternateList&& list)
    {
        m_paths = std::move(list.m_paths);
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateList<T>::GetSize() const
    {
        return std::get<Path<TPathType>>(m_paths).size;
    }

    template<typename T>
    inline size_t AlternateList<T>::GetMainSize() const
    {
        return std::get<Path<MainPath>>(m_paths).size;
    }

    template<typename T>
    inline size_t AlternateList<T>::GetSubSize() const
    {
        return std::get<Path<SubPath>>(m_paths).size;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateList<T>::template IteratorPath<TPathType> AlternateList<T>::GetPath()
    {
        return IteratorPath<TPathType>{ std::get<Path<TPathType>>(m_paths).root };
    }
    template<typename T>
    template<typename TPathType>
    inline const typename AlternateList<T>::template IteratorPath<TPathType> AlternateList<T>::GetPath() const
    {
        return IteratorPath<TPathType>{ std::get<Path<TPathType>>(m_paths).root };
    }

    template<typename T>
    inline typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::MainPath> AlternateList<T>::GetMainPath()
    {
        return IteratorPath<MainPath>{ std::get<Path<MainPath>>(m_paths).root };
    }
    template<typename T>
    inline const typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::MainPath> AlternateList<T>::GetMainPath() const
    {
        return IteratorPath<MainPath>{ std::get<Path<MainPath>>(m_paths).root };
    }

    template<typename T>
    inline typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::SubPath> AlternateList<T>::GetSubPath()
    {
        return IteratorPath<SubPath>{ std::get<Path<SubPath>>(m_paths).root };
    }
    template<typename T>
    inline const typename AlternateList<T>::template IteratorPath<typename AlternateList<T>::SubPath> AlternateList<T>::GetSubPath() const
    {
        return IteratorPath<SubPath>{ std::get<Path<SubPath>>(m_paths).root };
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
    inline AlternateList<T>::Path<TPathType>::Path() :
        root(nullptr),
        tail(nullptr),
        size(0)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateList<T>::Path<TPathType>::Path(Path&& path) :
        root(path.root),
        tail(path.tail),
        size(path.size)
    {
        path.root = nullptr;
        path.tail = nullptr;
        path.size = 0;
    }

    template<typename T>
    template<typename TPathType>
    typename AlternateList<T>::template Path<TPathType>& AlternateList<T>::Path<TPathType>::operator =(Path<TPathType>&& path)
    {
        root = path.root;
        tail = path.tail;
        size = path.size;
        path.root = nullptr;
        path.tail = nullptr;
        path.size = 0;
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalPushBack(AlternateListNode<T>* newNode)
    {
        auto& path = std::get<Path<TPathType>>(m_paths);

        auto oldTail = path.tail;
        AlternateListPathTraits<TPathType>::SetPrev(newNode, oldTail);

        path.tail = newNode;

        if (!path.root)
        {
            path.root = newNode;
        }

        if (oldTail)
        {
            AlternateListPathTraits<TPathType>::SetNext(oldTail, newNode);
        }

        path.size++;
    }

    template<typename T>
    template<typename TPathType>
    inline void AlternateList<T>::InternalPushFront(AlternateListNode<T>* newNode)
    {
        auto& path = std::get<Path<TPathType>>(m_paths);

        auto oldRoot = path.root;
        AlternateListPathTraits<TPathType>::SetNext(newNode, oldRoot);

        path.root = newNode;

        if (!path.tail)
        {
            path.tail = newNode;
        }

        if (oldRoot)
        {
            AlternateListPathTraits<TPathType>::SetPrev(oldRoot, newNode);
        }

        path.size++;
    }


    // Alterante list node implementations.
    template<typename T>
    inline AlternateListNode<T>::AlternateListNode() :
        value{},
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}

    template<typename T>
    inline AlternateListNode<T>::AlternateListNode(const Type& valueCopy) :
        value{ valueCopy },
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}

    template<typename T>
    inline AlternateListNode<T>::AlternateListNode(Type&& valueMove) :
        value{ std::move(valueMove) },
        prevMain(nullptr),
        prevSub(nullptr),
        nextMain(nullptr),
        nextSub(nullptr)
    {}


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
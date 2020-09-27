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

    // Alterante tree iterator implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::Iterator<TPathType>::Iterator() :
        m_node(nullptr),
        m_listIterator(nullptr)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::Iterator<TPathType>::Iterator(AlternateTreeNode<Type>* node, ListIterator<TPathType> listIterator) :
        m_node(node),
        m_listIterator(listIterator)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::Iterator<TPathType>::IsEmpty() const
    {
        return m_node == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline T& AlternateTree<T>::Iterator<TPathType>::GetValue()
    {
        return (*m_listIterator).GetValue();
    }

    template<typename T>
    template<typename TPathType>
    inline AlternateTreeNode<T>& AlternateTree<T>::Iterator<TPathType>::operator *() const
    {
        return *m_listIterator;
    }
    
    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType>& AlternateTree<T>::Iterator<TPathType>::operator ++ () // Pre
    {
        ++m_listIterator;
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType>& AlternateTree<T>::Iterator<TPathType>::operator -- () // Pre
    {
        
        --m_listIterator;
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType> AlternateTree<T>::Iterator<TPathType>::operator ++ (int) // Post
    {
        auto oldIterator = m_listIterator++;
        return Iterator{ m_node, oldIterator };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType> AlternateTree<T>::Iterator<TPathType>::operator -- (int) // Post
    {
        auto oldIterator = m_listIterator--;
        return Iterator{ m_node, oldIterator };
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::Iterator<TPathType>::operator == (const Iterator<TPathType>& rhs) const
    {
        return m_listIterator == rhs.m_listIterator;
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::Iterator<TPathType>::operator != (const Iterator<TPathType>& rhs) const
    {
        return m_listIterator != rhs.m_listIterator;
    }


    // Alterante tree const iterator implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::ConstIterator<TPathType>::ConstIterator() :
        m_node(nullptr),
        m_listIterator(nullptr)
    {}

    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::ConstIterator<TPathType>::ConstIterator(const AlternateTreeNode<Type>* node, ListConstIterator<TPathType> listIterator) :
        m_node(node),
        m_listIterator(listIterator)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::ConstIterator<TPathType>::IsEmpty() const
    {
        return m_node == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline const T& AlternateTree<T>::ConstIterator<TPathType>::GetValue() const
    {
        return (*m_listIterator).GetValue();
    }

    template<typename T>
    template<typename TPathType>
    inline const AlternateTreeNode<T>& AlternateTree<T>::ConstIterator<TPathType>::operator *() const
    {
        return *m_listIterator;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType>& AlternateTree<T>::ConstIterator<TPathType>::operator ++ () // Pre
    {
        ++m_listIterator;
        return *this;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType>& AlternateTree<T>::ConstIterator<TPathType>::operator -- () // Pre
    {

        --m_listIterator;
        return *this;
    }
    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType> AlternateTree<T>::ConstIterator<TPathType>::operator ++ (int) // Post
    {
        auto oldIterator = m_listIterator++;
        return ConstIterator{ m_node, oldIterator };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType> AlternateTree<T>::ConstIterator<TPathType>::operator -- (int) // Post
    {
        auto oldIterator = m_listIterator--;
        return ConstIterator{ m_node, oldIterator };
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::ConstIterator<TPathType>::operator == (const ConstIterator<TPathType>& rhs) const
    {
        return m_listIterator == rhs.m_listIterator;
    }

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::ConstIterator<TPathType>::operator != (const ConstIterator<TPathType>& rhs) const
    {
        return m_listIterator != rhs.m_listIterator;
    }


    // Alterante tree iterator path implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::IteratorPath<TPathType>::IteratorPath() :
        m_node(nullptr),
        m_path(nullptr)
    { }

    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::IteratorPath<TPathType>::IteratorPath(AlternateTreeNode<Type>* node, ListIteratorPath<TPathType> path) :
        m_node(node),
        m_path(path)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::IteratorPath<TPathType>::IsEmpty() const
    {
        return m_node == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateTree<T>::IteratorPath<TPathType>::GetSize() const
    {
        return m_path.GetSize();
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType> AlternateTree<T>::IteratorPath<TPathType>::begin()
    {
        return Iterator<TPathType>{ m_node, m_path.begin() };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType> AlternateTree<T>::IteratorPath<TPathType>::end()
    {
        return Iterator<TPathType>{ m_node, m_path.end() };
    }


    // Alterante tree const iterator path implementations.
    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::ConstIteratorPath<TPathType>::ConstIteratorPath() :
        m_node(nullptr),
        m_path(nullptr)
    { }

    template<typename T>
    template<typename TPathType>
    inline AlternateTree<T>::ConstIteratorPath<TPathType>::ConstIteratorPath(const AlternateTreeNode<Type>* node, ListConstIteratorPath<TPathType> path) :
        m_node(node),
        m_path(path)
    {}

    template<typename T>
    template<typename TPathType>
    inline bool AlternateTree<T>::ConstIteratorPath<TPathType>::IsEmpty() const
    {
        return m_node == nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateTree<T>::ConstIteratorPath<TPathType>::GetSize() const
    {
        return m_path.GetSize();
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType> AlternateTree<T>::ConstIteratorPath<TPathType>::begin()
    {
        return ConstIterator<TPathType>{ m_node, m_path.begin() };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template ConstIterator<TPathType> AlternateTree<T>::ConstIteratorPath<TPathType>::end()
    {
        return ConstIterator<TPathType>{ m_node, m_path.end() };
    }


    // Alterante tree implementations.
    template<typename T>
    inline AlternateTree<T>::AlternateTree() :
        m_root(nullptr)
    {}

    template<typename T>
    inline AlternateTree<T>::~AlternateTree()
    {}

    template<typename T>
    inline AlternateTreeNode<T>& AlternateTree<T>::GetRoot()
    {
        return m_root;
    }
    template<typename T>
    inline const AlternateTreeNode<T>& AlternateTree<T>::GetRoot() const
    {
        return m_root;
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTree<T>::template Iterator<TPathType> AlternateTree<T>::Erase(Iterator<TPathType> it)
    {
        auto* node = it.m_node;
        auto & children = node->m_children;
        auto newIt = children.Erase(it.m_listIterator);
        return Iterator<TPathType>{ node, newIt };
    }


    // Alterante tree node implementations.
    template<typename T>
    inline AlternateTreeNode<T>::~AlternateTreeNode()
    {}

    template<typename T>
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode&& node) noexcept :
        m_value(std::move(node.m_value)),
        m_parent(node.m_parent),
        m_children(std::move(node.m_children))
    {
        node.m_parent = nullptr;
    }

    template<typename T>
    inline AlternateTreeNode<T>& AlternateTreeNode<T>::operator =(AlternateTreeNode&& node) noexcept
    {
        m_value = std::move(node.m_value);
        m_parent = node.m_parent;
        m_children = std::move(node.m_children);

        node.m_parent = nullptr;

        return *this;
    }

    template<typename T>
    inline typename AlternateTreeNode<T>::Type& AlternateTreeNode<T>::GetValue()
    {
        return m_value;
    }
    template<typename T>
    inline const typename AlternateTreeNode<T>::Type& AlternateTreeNode<T>::GetValue() const
    {
        return m_value;
    }

    template<typename T>
    inline bool AlternateTreeNode<T>::HasParent() const
    {
        return m_parent != nullptr;
    }

    template<typename T>
    template<typename TPathType>
    inline size_t AlternateTreeNode<T>::GetSize() const
    {
        return m_children.template GetSize<TPathType>();
    }

    template<typename T>
    inline size_t AlternateTreeNode<T>::GetMainSize() const
    {
        return m_children.GetMainSize();
    }

    template<typename T>
    inline size_t AlternateTreeNode<T>::GetSubSize() const
    {
        return m_children.GetSubSize();
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTreeNode<T>::template IteratorPath<TPathType> AlternateTreeNode<T>::GetPath()
    {
        return IteratorPath<TPathType>{ this, m_children.template GetPath<TPathType>() };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTreeNode<T>::template ConstIteratorPath<TPathType> AlternateTreeNode<T>::GetPath() const
    {
        return ConstIteratorPath<TPathType>{ this, m_children.template GetPath<TPathType>() };
    }

    template<typename T>
    inline typename AlternateTreeNode<T>::template IteratorPath<AlternateListMainPath> AlternateTreeNode<T>::GetMainPath()
    {
        return IteratorPath<MainPath>{ this, m_children.GetMainPath() };
    }
    template<typename T>
    inline typename AlternateTreeNode<T>::template ConstIteratorPath<AlternateListMainPath> AlternateTreeNode<T>::GetMainPath() const
    {
        return ConstIteratorPath<MainPath>{ this, m_children.GetMainPath() };
    }

    template<typename T>
    inline typename AlternateTreeNode<T>::template IteratorPath<AlternateListSubPath> AlternateTreeNode<T>::GetSubPath()
    {
        return IteratorPath<SubPath>{ this, m_children.GetSubPath() };
    }
    template<typename T>
    inline typename AlternateTreeNode<T>::template ConstIteratorPath<AlternateListSubPath> AlternateTreeNode<T>::GetSubPath() const
    {
        return ConstIteratorPath<SubPath>{ this, m_children.GetSubPath() };
    }

    template<typename T>
    inline void AlternateTreeNode<T>::PushBack(const bool addSubPath, const Type& value)
    {
        m_children.PushBack(addSubPath, AlternateTreeNode(this, value));
    }

    template<typename T>
    inline void AlternateTreeNode<T>::PushBack(const bool addSubPath, Type&& value)
    {
        m_children.PushBack(addSubPath, AlternateTreeNode(this, std::move(value)));
    }

    template<typename T>
    inline void AlternateTreeNode<T>::PushFront(const bool addSubPath, const Type& value)
    {
        m_children.PushFront(addSubPath, AlternateTreeNode(this, value));
    }

    template<typename T>
    inline void AlternateTreeNode<T>::PushFront(const bool addSubPath, Type&& value)
    {
        m_children.PushFront(addSubPath, AlternateTreeNode(this, std::move(value)));
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTreeNode<T>::template Iterator<AlternateListMainPath> AlternateTreeNode<T>::Insert(Iterator<TPathType> position, const bool addSubPath, Type&& value)
    {
        auto insertIt = m_children.Insert(position.m_listIterator, addSubPath, std::move(value));
        return Iterator<MainPath>{ this, insertIt };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTreeNode<T>::template Iterator<AlternateListMainPath> AlternateTreeNode<T>::Insert(Iterator<TPathType> position, const bool addSubPath, const Type& value)
    {
        auto insertIt = m_children.Insert(position.m_listIterator, addSubPath, value);
        return Iterator<MainPath>{ this, insertIt };
    }

    template<typename T>
    template<typename TPathType>
    inline typename AlternateTreeNode<T>::template Iterator<TPathType> AlternateTreeNode<T>::Erase(Iterator<TPathType> it)
    {
        auto* node = it.m_node;
        auto& children = node->m_children;
        auto newIt = children.Erase(it.m_listIterator);
        return Iterator<TPathType>{ node, newIt };
    }

    template<typename T>
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode* parent) :
        m_value{},
        m_parent(parent)
    {}

    template<typename T>
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode* parent, const Type& value) :
        m_value(value),
        m_parent(parent)
    {}

    template<typename T>
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode* parent, Type&& value) :
        m_value(std::move(value)),
        m_parent(parent)
    {}

}
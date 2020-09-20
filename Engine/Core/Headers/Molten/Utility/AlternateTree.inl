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


    // Alterante tree node implementations.
    template<typename T>
    inline AlternateTreeNode<T>::~AlternateTreeNode()
    {}

    template<typename T>
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode&& node) :
        m_value(std::move(node.m_value)),
        m_parent(node.m_parent),
        m_children(std::move(node.m_children))
    {
        node.m_parent = nullptr;
    }

    template<typename T>
    inline AlternateTreeNode<T>& AlternateTreeNode<T>::operator =(AlternateTreeNode&& node)
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
        return m_children.template GetPath<TPathType>();
    }
    template<typename T>
    template<typename TPathType>
    inline const typename AlternateTreeNode<T>::template IteratorPath<TPathType> AlternateTreeNode<T>::GetPath() const
    {
        return m_children.template GetPath<TPathType>();
    }

    template<typename T>
    inline typename AlternateTreeNode<T>::template IteratorPath<AlternateListMainPath> AlternateTreeNode<T>::GetMainPath()
    {
        return m_children.GetMainPath();
    }
    template<typename T>
    inline const typename AlternateTreeNode<T>::template IteratorPath<AlternateListMainPath> AlternateTreeNode<T>::GetMainPath() const
    {
        return m_children.GetMainPath();
    }

    template<typename T>
    inline typename AlternateTreeNode<T>::template IteratorPath<AlternateListSubPath> AlternateTreeNode<T>::GetSubPath()
    {
        return m_children.GetSubPath();
    }
    template<typename T>
    inline const typename AlternateTreeNode<T>::template IteratorPath<AlternateListSubPath> AlternateTreeNode<T>::GetSubPath() const
    {
        return m_children.GetSubPath();
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
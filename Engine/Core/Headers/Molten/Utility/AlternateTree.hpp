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

#ifndef MOLTEN_CORE_UTILITY_ALTERNATETREE_HPP
#define MOLTEN_CORE_UTILITY_ALTERNATETREE_HPP

#include "Molten/Utility/AlternateList.hpp"

namespace Molten
{

    template<typename T>
    class AlternateTree;

    template<typename T>
    class AlternateTreeNode;

    template<typename T>
    struct AlternateTreeNodeData;

    /* This is a tree container, based of AlternateList. */
    template<typename T>
    class AlternateTree
    {

    public:

        using Type = T;
        using List = AlternateList<AlternateTreeNode<Type>>;
        using MainPath = AlternateListMainPath;
        using SubPath = AlternateListMainPath;
        template<typename TPathType>
        using ListIteratorPath = typename List::template IteratorPath<TPathType>;
        template<typename TPathType>
        using ListConstIteratorPath = typename List::template ConstIteratorPath<TPathType>;
        template<typename TPathType>
        using ListIterator = typename List::template Iterator<TPathType>;
        template<typename TPathType>
        using ListConstIterator = typename List::template ConstIterator<TPathType>;


        template<typename TPathType>
        class Iterator
        {

        public:

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = AlternateTreeNode<Type>;
            using difference_type = std::ptrdiff_t;
            using pointer = AlternateTreeNode<Type>*;
            using reference = AlternateTreeNode<Type>&;

            Iterator();
            Iterator(AlternateTreeNode<Type> * node, ListIterator<TPathType> listIterator);

            bool IsEmpty() const;

            Type& GetValue();

            AlternateTreeNode<Type>& operator *() const;
            Iterator& operator ++ ();
            Iterator& operator -- ();
            Iterator operator ++ (int);
            Iterator operator -- (int);
            bool operator == (const Iterator& rhs) const;
            bool operator != (const Iterator& rhs) const;

        private:

            AlternateTreeNode<Type>* m_node;
            ListIterator<TPathType> m_listIterator;

            friend class AlternateTree<T>;
            friend class AlternateTreeNode<T>;

        };


        template<typename TPathType>
        class ConstIterator
        {

        public:

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = AlternateTreeNode<Type>;
            using difference_type = std::ptrdiff_t;
            using pointer = const AlternateTreeNode<Type>*;
            using reference = const AlternateTreeNode<Type>&;

            ConstIterator();
            ConstIterator(const AlternateTreeNode<Type>* node, ListConstIterator<TPathType> listIterator);

            bool IsEmpty() const;

            const Type& GetValue() const;

            const AlternateTreeNode<Type>& operator *() const;
            ConstIterator& operator ++ ();
            ConstIterator& operator -- ();
            ConstIterator operator ++ (int);
            ConstIterator operator -- (int);
            bool operator == (const ConstIterator& rhs) const;
            bool operator != (const ConstIterator& rhs) const;

        private:

            const AlternateTreeNode<Type>* m_node;
            ListConstIterator<TPathType> m_listIterator;

            friend class AlternateTree<T>;
            friend class AlternateTreeNode<T>;

        };


        template<typename TPathType>
        class IteratorPath
        {

        public:

            IteratorPath();
            IteratorPath(AlternateTreeNode<Type>* node, ListIteratorPath<TPathType> path);

            bool IsEmpty() const;

            size_t GetSize() const;

            Iterator<TPathType> begin();
            Iterator<TPathType> end();

        private:

            AlternateTreeNode<Type>* m_node;
            ListIteratorPath<TPathType> m_path;

            friend class AlternateTree<T>;
            friend class AlternateTreeNode<T>;

        };


        template<typename TPathType>
        class ConstIteratorPath
        {

        public:

            ConstIteratorPath();
            ConstIteratorPath(const AlternateTreeNode<Type>* node, ListConstIteratorPath<TPathType> path);

            bool IsEmpty() const;

            size_t GetSize() const;

            ConstIterator<TPathType> begin();
            ConstIterator<TPathType> end();

        private:

            const AlternateTreeNode<Type>* m_node;
            ListConstIteratorPath<TPathType> m_path;

            friend class AlternateTree<T>;
            friend class AlternateTreeNode<T>;

        };


        AlternateTree();
        ~AlternateTree();

        AlternateTreeNode<Type>& GetRoot();
        const AlternateTreeNode<Type>& GetRoot() const;

        template<typename TPathType>
        Iterator<TPathType> Erase(Iterator<TPathType> it);

    private:

        AlternateTree(const AlternateTree&) = delete;
        AlternateTree(AlternateTree&&) = delete;
        AlternateTree& operator =(const AlternateTree&) = delete;
        AlternateTree& operator =(AlternateTree&&) = delete;

        AlternateTreeNode<Type> m_root;

    };


    template<typename T>
    class AlternateTreeNode
    {

    public:

        using Type = T;
        using List = AlternateList<AlternateTreeNode<Type>>;
        using MainPath = AlternateListMainPath;
        using SubPath = AlternateListSubPath;

        template<typename TPathType>
        using Iterator = typename AlternateTree<T>::template Iterator<TPathType>;
        template<typename TPathType>
        using IteratorPath = typename AlternateTree<T>::template IteratorPath<TPathType>;
        template<typename TPathType>
        using ConstIteratorPath = typename AlternateTree<T>::template ConstIteratorPath<TPathType>;

        ~AlternateTreeNode();
        AlternateTreeNode(AlternateTreeNode&& node) noexcept;

        AlternateTreeNode& operator =(AlternateTreeNode&& node) noexcept;

        Type& GetValue();
        const Type& GetValue() const;

        bool HasParent() const;

        template<typename TPathType>
        size_t GetSize() const;
        size_t GetMainSize() const;
        size_t GetSubSize() const;

        template<typename TPathType>
        IteratorPath<TPathType> GetPath();
        template<typename TPathType>
        ConstIteratorPath<TPathType> GetPath() const;

        IteratorPath<MainPath> GetMainPath();
        ConstIteratorPath<MainPath> GetMainPath() const;
        
        IteratorPath<SubPath> GetSubPath();
        ConstIteratorPath<SubPath> GetSubPath() const;

        void PushBack(const bool addSubPath, const Type& value);
        void PushBack(const bool addSubPath, Type&& value);
        
        void PushFront(const bool addSubPath, const Type& value);
        void PushFront(const bool addSubPath, Type&& value);

        template<typename TPathType>
        Iterator<MainPath> Insert(Iterator<TPathType> position, const bool addSubPath, const Type& value);
        template<typename TPathType>
        Iterator<MainPath> Insert(Iterator<TPathType> position, const bool addSubPath, Type&& value);

        template<typename TPathType>
        Iterator<TPathType> Erase(Iterator<TPathType> it);

    private:

        AlternateTreeNode(AlternateTreeNode* parent);
        AlternateTreeNode(AlternateTreeNode* parent, const Type& value);
        AlternateTreeNode(AlternateTreeNode* parent, Type&& value);

        AlternateTreeNode(const AlternateTreeNode&) = delete;
        AlternateTreeNode& operator =(const AlternateTreeNode&) = delete;

        Type m_value;
        AlternateTreeNode* m_parent;
        List m_children;

        friend class AlternateTree<T>;

    };

}

#include "Molten/Utility/AlternateTree.inl"

#endif
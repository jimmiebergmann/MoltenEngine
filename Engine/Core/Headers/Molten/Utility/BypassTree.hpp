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

    private:

        Item m_rootItem;

    };


    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeLaneInterface
    {

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

    };

    template<bool IsConst, typename TLaneType, typename T>
    class BypassTreeIteratorInterface
    {

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

        BypassTreeIteratorInterface(const BypassTreeIteratorInterface<IsConst, BypassTreePartialLane, Type>& it);
        BypassTreeIteratorInterface& operator = (const BypassTreeIteratorInterface<IsConst, BypassTreePartialLane, Type>& it);

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




    /*
    template<typename T>
    class AlternateTree;

    template<typename T>
    class AlternateTreeNode;

    template<typename T>
    struct AlternateTreeNodeData;

    // This is a tree container, based of AlternateList.
    template<typename T>
    class AlternateTree
    {

    public:

        using Type = T;
        using List = AlternateList<AlternateTreeNode<Type>>;
        using MainPath = AlternateListMainPath;
        using SubPath = AlternateListSubPath;
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

            Type& GetValue() const;

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

            AlternateTreeNode<Type>& GetNode();

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

            const AlternateTreeNode<Type>& GetNode() const;

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

        //List& GetRoot();
        //const List& GetRoot() const;

        template<typename TPathType>
        Iterator<MainPath> Insert(Iterator<TPathType> position, const bool addSubPath, const Type& value);
        template<typename TPathType>
        Iterator<MainPath> Insert(Iterator<TPathType> position, const bool addSubPath, Type&& value);

        template<typename TPathType>
        Iterator<TPathType> Erase(Iterator<TPathType> it);

        template<typename TPathType, typename TCallback>
        void ForEachPreorder(TCallback && callback);

    private:

        AlternateTree(const AlternateTree&) = delete;
        AlternateTree(AlternateTree&&) = delete;
        AlternateTree& operator =(const AlternateTree&) = delete;
        AlternateTree& operator =(AlternateTree&&) = delete;

        List m_children;

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
    */
}

#include "Molten/Utility/BypassTree.inl"

#endif
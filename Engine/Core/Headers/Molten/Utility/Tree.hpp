/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_UTILITY_TREE_HPP
#define MOLTEN_CORE_UTILITY_TREE_HPP

#include <type_traits>

namespace Molten
{

    template<typename T> class TreeNode;
    template<typename T> class TreeLane;
    template<typename T, bool VIsConst> class TreeLaneIteratorInterface;
    template<typename T> using TreeLaneIterator = TreeLaneIteratorInterface<T, false>;
    template<typename T> using TreeLaneConstIterator = TreeLaneIteratorInterface<T, true>;
    template<typename T> class Tree;
    template<typename T> class TreeTraversalCache4;

    enum class TreeTraversalPreResult
    {
        VisitChildren,
        VisitFirstChild,
        SkipChildren
    };

    enum class TreeTraversalPreChildResult
    {
        Visit,
        VisitSingle,
        Skip,
        SkipRemaining
    };

    template<typename T>
    class TreeNode
    {

    public:

        using Iterator = TreeLaneIterator<T>;
        using ConstIterator = TreeLaneConstIterator<T>;

        explicit TreeNode(TreeNode* parent);
        explicit TreeNode(TreeNode* parent, const T& data);
        explicit TreeNode(TreeNode* parent, T&& data);

        ~TreeNode() = default;

        TreeNode(const TreeNode&) = delete;
        TreeNode(TreeNode&&) = delete;
        TreeNode& operator = (const TreeNode&) = delete;
        TreeNode& operator = (TreeNode&&) = delete;

        [[nodiscard]] T& GetData();
        [[nodiscard]] const T& GetData() const;

        [[nodiscard]] TreeNode<T>* GetParent();
        [[nodiscard]] const TreeNode<T>* GetParent() const;

        [[nodiscard]] TreeLane<T>& GetChildren();
        [[nodiscard]] const TreeLane<T>& GetChildren() const;

        [[nodiscard]] Iterator begin();
        [[nodiscard]] ConstIterator begin() const;
        [[nodiscard]] Iterator end();
        [[nodiscard]] ConstIterator end() const;

    private:

        template<typename TOtherT> friend class Tree;
        template<typename TOtherT, bool VIsConst> friend class TreeLaneIteratorInterface;

        TreeNode<T>* m_next;
        TreeNode<T>* m_prev;
        TreeNode<T>* m_parent;
        TreeLane<T> m_children;
        T m_data;

    };


    template<typename T>
    class TreeLane
    {

    public:

        using Iterator = TreeLaneIterator<T>;
        using ConstIterator = TreeLaneConstIterator<T>;

        explicit TreeLane(TreeNode<T>* parent);
        ~TreeLane() = default;

        TreeLane(const TreeLane&) = delete;
        TreeLane(TreeLane&&) = delete;
        TreeLane& operator = (const TreeLane&) = delete;
        TreeLane& operator = (TreeLane&&) = delete;

        [[nodiscard]] TreeNode<T>* GetParent();
        [[nodiscard]] const TreeNode<T>* GetParent() const;

        [[nodiscard]] Iterator begin();
        [[nodiscard]] ConstIterator begin() const;
        [[nodiscard]] Iterator end();
        [[nodiscard]] ConstIterator end() const;

        template<typename TPreCallback, typename TPostCallback, typename TPreChildCallback, typename TPostChildCallback>
        void ForEachPreorder(
            TPreCallback&& preCallback,
            TPostCallback&& postCallback,
            TPreChildCallback&& preChildCallback,
            TPostChildCallback&& postChildCallback);

        template<typename TPreCallback, typename TPostCallback, typename TPreChildCallback, typename TPostChildCallback>
        void ForEachPreorder(
            TreeTraversalCache4<T>& cache,
            TPreCallback&& preCallback,
            TPostCallback&& postCallback,
            TPreChildCallback&& preChildCallback,
            TPostChildCallback&& postChildCallback);

    private:

        template<typename TOtherT> friend class Tree;
        template<typename TOtherT, bool VIsConst> friend class TreeLaneIteratorInterface;

        TreeNode<T>* m_root;
        TreeNode<T>* m_tail;
        TreeNode<T>* m_parent;

    };


    template<typename T, bool VIsConst>
    class TreeLaneIteratorInterface
    {

    public:
        
        using Type = std::conditional_t<VIsConst, const TreeNode<T>, TreeNode<T>>;
        using Reference = Type&;
        using Pointer = Type*;

        TreeLaneIteratorInterface();
        TreeLaneIteratorInterface(Pointer node, TreeLane<T>* lane);

        [[nodiscard]] Reference operator *();
        [[nodiscard]] Pointer operator ->();

        TreeLaneIteratorInterface& operator ++ ();
        TreeLaneIteratorInterface& operator -- ();
        [[nodiscard]] TreeLaneIteratorInterface operator ++ (int);
        [[nodiscard]] TreeLaneIteratorInterface operator -- (int);
        [[nodiscard]] bool operator == (const TreeLaneIteratorInterface& rhs) const;
        [[nodiscard]] bool operator != (const TreeLaneIteratorInterface& rhs) const;

    private:

        template<typename TOtherT> friend class Tree;

        Pointer m_node;
        TreeLane<T>* m_lane;

    };


    template<typename T>
    class Tree
    {

    public:

        using Iterator = TreeLaneIterator<T>;
        using ConstIterator = TreeLaneConstIterator<T>;
        using Lane = TreeLane<T>;

        Tree();
        ~Tree();

        Tree(const Tree&) = delete;
        Tree(Tree&&) = delete;
        Tree& operator = (const Tree&) = delete;
        Tree& operator = (Tree&&) = delete;

        Iterator Insert(Iterator position, const T& value);
        Iterator Insert(Iterator position, T&& value);

        Iterator Erase(Iterator position);

        [[nodiscard]] TreeLane<T>& GetChildren();
        [[nodiscard]] const TreeLane<T>& GetChildren() const;

        [[nodiscard]] Iterator begin();
        [[nodiscard]] ConstIterator begin() const;
        [[nodiscard]] Iterator end();
        [[nodiscard]] ConstIterator end() const;

    private:

        Iterator InternalInsert(Iterator position, TreeNode<T>* newNode);
        void InternalInsertBefore(Iterator position, TreeNode<T>* newNode);
        void InternalInsertBack(TreeLane<T>* lane, TreeNode<T>* newNode);

        void EraseLane(TreeLane<T>& lane);

        TreeLane<T> m_children;

    };


    template<typename T>
    class TreeTraversalCache4
    {

    public:

        struct State
        {
            State(
                TreeNode<T>* parent,
                TreeLaneIterator<T> current,
                TreeLaneIterator<T> end);

            TreeNode<T>* parent;
            TreeLaneIterator<T> current;
            TreeLaneIterator<T> end;
        };

        TreeTraversalCache4() = default;
        TreeTraversalCache4(const size_t preallocatedStates);

        std::vector<State> states;

    };

}

#include "Molten/Utility/Tree.inl"

#endif
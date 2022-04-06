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

#include "Molten/Memory/Pointer.hpp"
#include "Molten/Memory/Reference.hpp"

namespace Molten
{

    // Tree node implementations
    template<typename T> 
    TreeNode<T>::TreeNode(TreeNode* parent) :
        m_next(nullptr),
        m_prev(nullptr),
        m_parent(parent),
        m_children(this),
        m_data{}
    {}

    template<typename T> 
    TreeNode<T>::TreeNode(TreeNode* parent, const T& data) :
        m_next(nullptr),
        m_prev(nullptr),
        m_parent(parent),
        m_children(this),
        m_data(data)
    {}

    template<typename T> 
    TreeNode<T>::TreeNode(TreeNode* parent, T&& data) :
        m_next(nullptr),
        m_prev(nullptr),
        m_parent(parent),
        m_children(this),
        m_data(std::move(data))
    {}

    template<typename T>
    T& TreeNode<T>::GetData()
    {
        return m_data;
    }
    template<typename T>
    const T& TreeNode<T>::GetData() const
    {
        return m_data;
    }

    template<typename T>
    TreeNode<T>* TreeNode<T>::GetParent()
    {
        return m_parent;
    }
    template<typename T>
    const TreeNode<T>* TreeNode<T>::GetParent() const
    {
        return m_parent;
    }

    template<typename T>
    TreeLane<T>& TreeNode<T>::GetChildren()
    {
        return m_children;
    }
    template<typename T>
    const TreeLane<T>& TreeNode<T>::GetChildren() const
    {
        return m_children;
    }

    template<typename T>
    typename TreeNode<T>::Iterator TreeNode<T>::begin()
    {
        return m_children.begin();
    }
    template<typename T>
    typename TreeNode<T>::ConstIterator TreeNode<T>::begin() const
    {
        return m_children.begin();
    }

    template<typename T>
    typename TreeNode<T>::Iterator TreeNode<T>::end()
    {
        return m_children.end();
    }
    template<typename T>
    typename TreeNode<T>::ConstIterator TreeNode<T>::end() const
    {
        return m_children.end();
    }


    // Tree lane implementations.
    template<typename T>
    TreeLane<T>::TreeLane(TreeNode<T>* parent) :
        m_root(nullptr),
        m_tail(m_root),
        m_parent(parent)
    {}

    template<typename T>
    TreeNode<T>* TreeLane<T>::GetParent()
    {
        return m_parent;
    }
    template<typename T>
    const TreeNode<T>* TreeLane<T>::GetParent() const
    {
        return m_parent;
    }

    template<typename T>
    typename TreeLane<T>::Iterator TreeLane<T>::begin()
    {
        return Iterator{ m_root, this };
    }
    template<typename T>
    typename TreeLane<T>::ConstIterator TreeLane<T>::begin() const
    {
        return ConstIterator{ m_root, const_cast<TreeLane*>(this) };
    }

    template<typename T>
    typename TreeLane<T>::Iterator TreeLane<T>::end()
    {
        return Iterator{ nullptr, this };
    }
    template<typename T>
    typename TreeLane<T>::ConstIterator TreeLane<T>::end() const
    {
        return ConstIterator{ nullptr, const_cast<TreeLane*>(this) };
    }

    template<typename T>
    template<typename TPreCallback, typename TPostCallback, typename TPreChildCallback, typename TPostChildCallback>
    void TreeLane<T>::ForEachPreorder(
        TPreCallback&& preCallback,
        TPostCallback&& postCallback,
        TPreChildCallback&& preChildCallback,
        TPostChildCallback&& postChildCallback)
    {
        TreeTraversalCache4<T> cache;
        ForEachPreorder(cache, preCallback, postCallback, preChildCallback, postChildCallback);
    }

    template<typename T>
    template<typename TPreCallback, typename TPostCallback, typename TPreChildCallback, typename TPostChildCallback>
    void TreeLane<T>::ForEachPreorder(
        TreeTraversalCache4<T>& /*cache*/,
        TPreCallback&& preCallback,
        TPostCallback&& postCallback,
        TPreChildCallback&& preChildCallback,
        TPostChildCallback&& postChildCallback)
    {
        // TODO: Make iterative instead of recursive.

        auto invokePreCallback = [&](auto& parent) -> TreeTraversalPreResult
        {
            if constexpr (std::is_same_v<std::invoke_result_t<decltype(preCallback), TreeNode<T>&>, TreeTraversalPreResult> == true)
            {
                return preCallback(parent);
            }
            else
            {
                preCallback(parent);
                return TreeTraversalPreResult::VisitChildren;
            }
        };

        auto invokePreChildCallback = [&](auto& parent, auto& child) -> TreeTraversalPreChildResult
        {
            if constexpr (std::is_same_v<std::invoke_result_t<decltype(preChildCallback), TreeNode<T>&, TreeNode<T>&>, TreeTraversalPreChildResult> == true)
            {
                return preChildCallback(parent, child);
            }
            else
            {
                preChildCallback(parent, child);
                return TreeTraversalPreChildResult::Visit;
            }
        };

        std::function<void(TreeNode<T>&)> traverseParent;
        std::function<void(TreeNode<T>&)> traverseChildren;
        std::function<void(TreeNode<T>&)> traverseFirstChild;

        traverseChildren = [&](TreeNode<T>& parent)
        {
            auto preChildResult = TreeTraversalPreChildResult::Visit;

            for (auto& child : parent.GetChildren())
            {
                if (preChildResult == TreeTraversalPreChildResult::VisitSingle)
                {
                    break;
                }

                preChildResult = invokePreChildCallback(parent, child);
                if (preChildResult == TreeTraversalPreChildResult::Visit || preChildResult == TreeTraversalPreChildResult::VisitSingle)
                {
                    traverseParent(child);
                    postChildCallback(parent, child);
                }
                else if (preChildResult == TreeTraversalPreChildResult::SkipRemaining)
                {
                    break;
                }
            }
        };

        traverseFirstChild = [&](TreeNode<T>& parent)
        {
            auto& children = parent.GetChildren();
            auto it = children.begin();
            if(it == children.end())
            {
                return;
            }

            auto& child = *it;

            auto preChildResult = invokePreChildCallback(parent, child);
            if (preChildResult == TreeTraversalPreChildResult::Visit || preChildResult == TreeTraversalPreChildResult::VisitSingle)
            {
                traverseParent(child);
                postChildCallback(parent, child);
            }

        };

        traverseParent = [&](TreeNode<T>& parent)
        {
            switch (invokePreCallback(parent))
            {
                case TreeTraversalPreResult::VisitChildren: traverseChildren(parent); break;
                case TreeTraversalPreResult::VisitFirstChild: traverseFirstChild(parent); break;
                case TreeTraversalPreResult::SkipChildren: break;
            }

            postCallback(parent);
        };

        for (auto& child : *this)
        {
            traverseParent(child);
        }
    }


    // Tree lane iterator interface implementations.
    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst>::TreeLaneIteratorInterface() :
        m_node(nullptr),
        m_lane(nullptr)
    {}

    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst>::TreeLaneIteratorInterface(Pointer node, TreeLane<T>* lane) :
        m_node(node),
        m_lane(lane)
    {}

    template<typename T, bool VIsConst>
    typename TreeLaneIteratorInterface<T, VIsConst>::Reference TreeLaneIteratorInterface<T, VIsConst>::operator *()
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot dereference end iterator of Tree<T>.");
        return *m_node;
    }

    template<typename T, bool VIsConst>
    typename TreeLaneIteratorInterface<T, VIsConst>::Pointer TreeLaneIteratorInterface<T, VIsConst>::operator ->()
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot dereference end iterator of Tree<T>.");
        return m_node;
    }

    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst>& TreeLaneIteratorInterface<T, VIsConst>::operator ++ ()
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot pre increment end iterator of Tree<T>.");
        MOLTEN_DEBUG_ASSERT(m_lane != nullptr, "Cannot pre increment iterator of Tree<T> with unknown lane.");

        m_node = m_node->m_next;
        return *this;
    }

    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst>& TreeLaneIteratorInterface<T, VIsConst>::operator -- ()
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot pre decrement end iterator of Tree<T>.");
        MOLTEN_DEBUG_ASSERT(m_lane != nullptr, "Cannot pre decrement iterator of Tree<T> with unknown lane.");

        m_node = m_node->m_prev;
        return *this;
    }

    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst> TreeLaneIteratorInterface<T, VIsConst>::operator ++ (int)
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot post increment end iterator of Tree<T>.");
        MOLTEN_DEBUG_ASSERT(m_lane != nullptr, "Cannot post increment iterator of Tree<T> with unknown lane.");

        auto* oldNode = m_node;
        m_node = m_node->m_next;

        return { oldNode, m_lane };
    }

    template<typename T, bool VIsConst>
    TreeLaneIteratorInterface<T, VIsConst> TreeLaneIteratorInterface<T, VIsConst>::operator -- (int)
    {
        MOLTEN_DEBUG_ASSERT(m_node != nullptr, "Cannot post decrement end iterator of Tree<T>.");
        MOLTEN_DEBUG_ASSERT(m_lane != nullptr, "Cannot post decrement iterator of Tree<T> with unknown lane.");

        auto* oldNode = m_node;
        m_node = m_node->m_prev;
        return { oldNode, m_lane };
    }

    template<typename T, bool VIsConst>
    bool TreeLaneIteratorInterface<T, VIsConst>::operator == (const TreeLaneIteratorInterface& rhs) const
    {
        return m_node == rhs.m_node && m_lane == rhs.m_lane;
    }

    template<typename T, bool VIsConst>
    bool TreeLaneIteratorInterface<T, VIsConst>::operator != (const TreeLaneIteratorInterface& rhs) const
    {
        return m_node != rhs.m_node || m_lane != rhs.m_lane;
    }


    // Tree implementations.
    template<typename T>
    Tree<T>::Tree() :
        m_children(nullptr)
    {}

    template<typename T>
    Tree<T>::~Tree()
    {
        EraseLane(m_children);
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::Insert(Iterator position, const T& value)
    {
        MOLTEN_DEBUG_ASSERT(position.m_lane != nullptr, "Cannot insert at position with unknown lane.");

        auto* newNode = new TreeNode<T>(position.m_lane->m_parent, value);
        return InternalInsert(position, newNode);
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::Insert(Iterator position, T&& value)
    {
        MOLTEN_DEBUG_ASSERT(position.m_lane != nullptr, "Cannot insert at position with unknown lane.");

        auto* newNode = new TreeNode<T>(position.m_lane->m_parent, std::move(value));
        return InternalInsert(position, newNode);
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::Erase(Iterator position)
    {
        MOLTEN_DEBUG_ASSERT(position.m_node != nullptr, "Cannot erase end iterator of Tree<T>.");
        MOLTEN_DEBUG_ASSERT(position.m_lane != nullptr, "Cannot erase at position with unknown lane.");

        auto* inLane = position.m_lane;
        auto* eraseNode = position.m_node;

        auto* nextNode = eraseNode->m_next;

        if(nextNode)
        {
            nextNode->m_prev = eraseNode->m_prev;
        }
        if (auto* prevNode = eraseNode->m_prev; prevNode)
        {
            prevNode->m_next = eraseNode->m_next;
        }

        if (eraseNode == inLane->m_root)
        {
            inLane->m_root = eraseNode->m_next;
        }
        if (eraseNode == inLane->m_tail)
        {
            inLane->m_tail = eraseNode->m_prev;
        }

        EraseLane(eraseNode->GetChildren());
        delete eraseNode;

        return { nextNode, inLane };
    }

    template<typename T>
    TreeLane<T>& Tree<T>::GetChildren()
    {
        return m_children;
    }
    template<typename T>
    const TreeLane<T>& Tree<T>::GetChildren() const
    {
        return m_children;
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::begin()
    {
        return m_children.begin();
    }
    template<typename T>
    typename Tree<T>::ConstIterator Tree<T>::begin() const
    {
        return m_children.begin();
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::end()
    {
        return m_children.end();
    }
    template<typename T>
    typename Tree<T>::ConstIterator Tree<T>::end() const
    {
        return m_children.end();
    }

    template<typename T>
    typename Tree<T>::Iterator Tree<T>::InternalInsert(Iterator position, TreeNode<T>* newNode)
    {
        if (position.m_node)
        {
            InternalInsertBefore(position, newNode);
        }
        else
        {
            InternalInsertBack(position.m_lane, newNode);
        }

        return { newNode, position.m_lane };
    }

    template<typename T>
    void Tree<T>::InternalInsertBefore(Iterator position, TreeNode<T>* newNode)
    {
        auto* atNode = position.m_node;

        if(atNode == nullptr)
        {
            InternalInsertBack(position.m_lane, newNode);
        }
        else
        {
            auto* inLane = position.m_lane;

            newNode->m_next = atNode;
            newNode->m_prev = atNode->m_prev;

            if(auto* prevNode = atNode->m_prev; prevNode)
            {
                prevNode->m_next = newNode;
            }

            atNode->m_prev = newNode;

            if(atNode == inLane->m_root)
            {
                inLane->m_root = newNode;
            }
        }
    }

    template<typename T>
    void Tree<T>::InternalInsertBack(TreeLane<T>* lane, TreeNode<T>* newNode)
    {
        auto* oldTail = lane->m_tail;

        lane->m_tail = newNode;
        newNode->m_prev = oldTail;

        if(oldTail)
        {
            oldTail->m_next = newNode;
        }
        else
        {
            lane->m_root = newNode;
        }
    }

    template<typename T>
    void Tree<T>::EraseLane(TreeLane<T>& lane)
    {
        // TODO: Implement iterative instead of recursive loop...

        std::function<void(TreeLane<T>&)> cleanChild;
        cleanChild = [&](TreeLane<T>& children)
        {
            auto* current = children.m_tail;
            while (current)
            {
                cleanChild(current->GetChildren());

                auto* prev = current;
                current = current->m_prev;
                delete prev;
            }
        };

        cleanChild(lane);
    }


    // Tree traversal cache 4 implementations.
    template<typename T>
    TreeTraversalCache4<T>::State::State(
        TreeNode<T>* parent,
        TreeLaneIterator<T> current,
        TreeLaneIterator<T> end
    ) :
        parent(parent),
        current(current),
        end(end)
    {}

    template<typename T>
    TreeTraversalCache4<T>::TreeTraversalCache4(const size_t preallocatedStates)
    {
        states.reserve(preallocatedStates);
    }


}

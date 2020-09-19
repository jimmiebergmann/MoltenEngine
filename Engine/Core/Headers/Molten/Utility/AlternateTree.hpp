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
#include <tuple>

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
        using MainPath = AlternateListMainPath;
        using SubPath = AlternateListMainPath;

        AlternateTree();
        ~AlternateTree();

        AlternateTreeNode<T>& GetRoot();
        const AlternateTreeNode<T>& GetRoot() const;


    private:

        AlternateTree(const AlternateTree&) = delete;
        AlternateTree(AlternateTree&&) = delete;
        AlternateTree& operator =(const AlternateTree&) = delete;
        AlternateTree& operator =(AlternateTree&&) = delete;

        AlternateTreeNode<T> m_root;

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
        using IteratorPath = typename List::template IteratorPath<TPathType>;

        AlternateTreeNode(AlternateTreeNode * parent);
        ~AlternateTreeNode();

       /* template<typename TPathType>
        IteratorPath<TPathType> GetPath();
        template<typename TPathType>
        const IteratorPath<TPathType> GetPath() const;*/
        
       // IteratorPath<AlternateListMainPath> GetMainPath();
        /*const IteratorPath<MainPath> GetMainPath() const;

        IteratorPath<SubPath> GetSubPath();
        const IteratorPath<SubPath> GetSubPath() const;*/

    private:

        AlternateTreeNode(const AlternateTreeNode&) = delete;
        AlternateTreeNode(AlternateTreeNode&&) = delete;
        AlternateTreeNode& operator =(const AlternateTreeNode&) = delete;
        AlternateTreeNode& operator =(AlternateTreeNode&&) = delete;

        AlternateTreeNode* m_parent;
        List m_list;

    };

}

#include "Molten/Utility/AlternateTree.inl"

#endif
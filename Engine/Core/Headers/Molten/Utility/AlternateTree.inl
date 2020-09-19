#include "AlternateTree.hpp"
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
    inline AlternateTreeNode<T>::AlternateTreeNode(AlternateTreeNode* parent) :
        m_parent(parent)
    {}

    template<typename T>
    inline AlternateTreeNode<T>::~AlternateTreeNode()
    {}

   /* template<typename T>
    template<typename TPathType>
    inline AlternateTreeNode<T>::IteratorPath<TPathType> AlternateTreeNode<T>::GetPath()
    {
        return m_list.GetPath<TPathType>();
    }
    template<typename T>
    template<typename TPathType>
    inline const AlternateTreeNode<T>::IteratorPath<TPathType> AlternateTreeNode<T>::GetPath() const
    {
        return m_list.GetPath<TPathType>();
    }*/

    // AlternateListMainPath

    template<typename T>
    inline typename AlternateTreeNode<T>::template IteratorPath<typename AlternateTreeNode<T>::MainPath> AlternateTreeNode<T>::GetMainPath()
    {
        return {};//IteratorPath<MainPath>{ std::get<Path<MainPath>>(m_paths).root };
    }

    /*template<typename T>
    inline AlternateTreeNode<T>::IteratorPath<typename AlternateTreeNode<T>::MainPath> AlternateTreeNode<T>::GetMainPath()
    {
        return {};// m_list.GetPath<typename AlternateTreeNode<T>::MainPath>();
    }*/

    /*template<typename T>
    inline typename AlternateTreeNode<T>::IteratorPath<typename AlternateTreeNode<T>::MainPath> AlternateTreeNode<T>::GetMainPath()
    {
        //return m_list.GetMainPath();
    }*/

    /*template<typename T>
    inline const AlternateTreeNode<T>::IteratorPath<typename AlternateTreeNode<T>::MainPath> AlternateTreeNode<T>::GetMainPath() const
    {
        return m_list.GetMainPath();
    }*/

    /*template<typename T>
    AlternateTreeNode<T>::IteratorPath<typename AlternateTreeNode<T>::SubPath> AlternateTreeNode<T>::GetSubPath()
    {
        return m_list.GetSubPath();
    }
    template<typename T>
    const AlternateTreeNode<T>::IteratorPath<typename AlternateTreeNode<T>::SubPath> AlternateTreeNode<T>::GetSubPath() const
    {
        return m_list.GetSubPath();
    }*/

}
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

#ifndef MOLTEN_CORE_UTILITY_ALTERNATELIST_HPP
#define MOLTEN_CORE_UTILITY_ALTERNATELIST_HPP

#include "Molten/Types.hpp"
#include <tuple>

namespace Molten
{

    template<typename T>
    class AlternateList;

    template<typename T>
    class AlternateListNode;

    template<typename T, typename TPathType>
    struct AlternateListPath;

    struct AlternateListMainPath {};

    struct AlternateListSubPath {};

    /*
    * This is a list container of two possible paths, main and sub path.
    * Main path contains the entire list of nodes and the subpath contains only selected nodes.
    * It is possible to add or remove a node from the main path to/from the sub path.
    * This container is suitable for storing a list of nodes, where a fast iteration of enabled nodes is needed,
    * for example an GUI tree, where some nodes are disabled, thus not present in the sub path.
    */
    template<typename T>
    class AlternateList
    {

    public:

        using Type = T;
        using MainPath = AlternateListMainPath;
        using SubPath = AlternateListSubPath;

        template<typename TPathType>
        class Iterator
        {

        public:

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = Type;
            using difference_type = std::ptrdiff_t;
            using pointer = Type*;
            using reference = Type&;

            Iterator();
            Iterator(AlternateListNode<T>* currentNode);

            Type& operator *() const;
            Iterator& operator ++ (); 
            Iterator& operator -- ();
            Iterator operator ++ (int);
            Iterator operator -- (int);
            bool operator == (const Iterator& rhs) const;
            bool operator != (const Iterator& rhs) const;

        private: 

            AlternateListNode<T>* m_currentNode;

            friend class AlternateList<T>;

        };

        template<typename TPathType>
        class ConstIterator
        {

        public:

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = Type;
            using difference_type = std::ptrdiff_t;
            using pointer = const Type*;
            using reference = const Type&;

            ConstIterator();
            ConstIterator(const AlternateListNode<T>* currentNode);

            const Type& operator *() const;
            ConstIterator& operator ++ ();
            ConstIterator& operator -- ();
            ConstIterator operator ++ (int);
            ConstIterator operator -- (int);
            bool operator == (const ConstIterator& rhs) const;
            bool operator != (const ConstIterator& rhs) const;

        private:

            const AlternateListNode<T>* m_currentNode;

            friend class AlternateList<T>;

        };

        template<typename TPathType>
        class IteratorPath
        {

        public:

            IteratorPath();
            IteratorPath(AlternateList<Type>* list);

            Iterator<TPathType> begin();
            ConstIterator<TPathType> begin() const;

            Iterator<TPathType> end();
            ConstIterator<TPathType> end() const;

        private:

            AlternateList<Type>* m_list;

            friend class AlternateList<Type>;

        };

        AlternateList();
        ~AlternateList();
        AlternateList(AlternateList&& list);

        AlternateList& operator =(AlternateList&& list);

        template<typename TPathType>
        size_t GetSize() const;
        size_t GetMainSize() const;
        size_t GetSubSize() const;

        template<typename TPathType>
        IteratorPath<TPathType> GetPath();
        template<typename TPathType>
        const IteratorPath<TPathType> GetPath() const;

        IteratorPath<MainPath> GetMainPath();
        const IteratorPath<MainPath> GetMainPath() const;

        IteratorPath<SubPath> GetSubPath();
        const IteratorPath<SubPath> GetSubPath() const;

        void PushBack(const bool addSubPath, const Type& value);
        void PushBack(const bool addSubPath, Type&& value);    

        void PushFront(const bool addSubPath, const Type& value);
        void PushFront(const bool addSubPath, Type&& value);    

        template<typename TPathType>
        Iterator<TPathType> Erase(Iterator<TPathType> it);

        /*
        * TODO:
        void Insert(Iterator it, const bool addSubPath, Type&& value);
        void Insert(Iterator it, const bool addSubPath, const Type& value);
        */

    private:

        AlternateList(const AlternateList&) = delete;
        AlternateList& operator =(const AlternateList&) = delete;

        template<typename TPathType>
        void InternalPushBack(AlternateListNode<T>* node);

        template<typename TPathType>
        void InternalPushFront(AlternateListNode<T>* node);

        template<typename TPathType>
        void InternalErase(AlternateListNode<T>* node);

        bool InternalIsInSubPath(AlternateListNode<T>* node);

        AlternateListNode<Type>* m_endNode;
        std::tuple<AlternateListPath<Type, MainPath>, AlternateListPath<Type, SubPath>> m_paths;

        friend class IteratorPath<MainPath>;
        friend class IteratorPath<SubPath>;

    };


    template<typename T>
    class AlternateListNode
    {

    public:

        using Type = T;

        AlternateListNode();
        AlternateListNode(const Type& valueCopy);
        AlternateListNode(Type&& valueMove);
        ~AlternateListNode();

        struct Content
        {
            Content(const Type& valueCopy);
            Content(Type&& valueMove);
            Type value;
        };

        Content* content;
        AlternateListNode* prevMain;
        AlternateListNode* prevSub;
        AlternateListNode* nextMain;
        AlternateListNode* nextSub;

    private:

        AlternateListNode(const AlternateListNode&) = delete;
        AlternateListNode(AlternateListNode&&) = delete;
        AlternateListNode& operator =(const AlternateListNode&) = delete;
        AlternateListNode& operator =(AlternateListNode&&) = delete;
    };


    template<typename T, typename TPathType>
    struct AlternateListPath
    {
        AlternateListPath(AlternateListNode<T>* end);
        AlternateListPath(AlternateListPath&& path);
        AlternateListPath& operator =(AlternateListPath&& path);

        AlternateListPath(const AlternateListPath&) = delete;
        AlternateListPath& operator =(const AlternateListPath&) = delete;


        AlternateListNode<T>* root;
        AlternateListNode<T>* tail;
        size_t size;
    };


    template<typename TPathType>
    struct AlternateListPathTraits
    {

        template<typename T>
        static AlternateListNode<T>* GetPrev(AlternateListNode<T>* node);

        template<typename T>
        static AlternateListNode<T>* GetNext(AlternateListNode<T>* node);

        template<typename T>
        static void SetPrev(AlternateListNode<T>* node, AlternateListNode<T>* prev);

        template<typename T>
        static void SetNext(AlternateListNode<T>* node, AlternateListNode<T>* next);

    };
    

}

#include "Molten/Utility/AlternateList.inl"

#endif
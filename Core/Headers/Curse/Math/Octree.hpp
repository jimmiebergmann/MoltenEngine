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

#ifndef CURSE_CORE_MATH_OCTREE_HPP
#define CURSE_CORE_MATH_OCTREE_HPP

#include "Curse/Math/AxisAlignedBoundingBox.hpp"
#include <functional>
#include <array>

namespace Curse
{

    template<typename ObjectT, typename CoordinateT = float>
    class OctreeAABB
    {

    public:

        using VectorType = Vector3<CoordinateT>;
        using AabbType = Aabb3<CoordinateT>;

        OctreeAABB(const VectorType& position, const VectorType size, const size_t maxLevels);
        ~OctreeAABB();

        bool Insert(ObjectT& object, const AabbType& aabb, const VectorType& position = { 0, 0, 0 });

        bool ForEachNode(std::function<void(AabbType)> callback);

    private:

        OctreeAABB(const OctreeAABB&) = delete;
        OctreeAABB(OctreeAABB&&) = delete;

        struct Node
        {

            using NodePtr = std::unique_ptr<Node>;
            using NodeObjects = std::vector<ObjectT*>;

            explicit Node(const AabbType& aabb);

            AabbType aabb;
            std::array<NodePtr, 8> childNodes;
            size_t usedChildNodes;
            std::unique_ptr<NodeObjects> objects;

        };

        size_t m_maxLevels;
        Node m_rootNode;

    };

}

#include "Curse/Math/Octree.inl"

#endif
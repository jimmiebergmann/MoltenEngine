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

#include <stack>

namespace Curse
{

    // Axis aligned bounding box octree implementations.
    template<typename ObjectT, typename CoordinateT>
    inline OctreeAABB<ObjectT, CoordinateT>::OctreeAABB(const VectorType& position, const VectorType size, const size_t maxLevels) :
        m_maxLevels(maxLevels),
        m_rootNode{ AabbType{ (size / static_cast<CoordinateT>(-2)) + position,
                              (size / static_cast<CoordinateT>( 2)) + position } }
    { }

    template<typename ObjectT, typename CoordinateT>
    inline OctreeAABB<ObjectT, CoordinateT>::~OctreeAABB()
    {

    }

    template<typename ObjectT, typename CoordinateT>
    inline bool OctreeAABB<ObjectT, CoordinateT>::Insert(ObjectT& object, const AabbType& aabb, const VectorType& position)
    {
        AabbType newAabb = aabb + position;
        newAabb.FixMinMax();

        if (!m_rootNode.aabb.Intersects(newAabb))
        {
            return false;
        }

        using StackItem = std::pair<Node*, size_t>;
        std::stack<StackItem> nodeStack;
        nodeStack.push({&m_rootNode, 0});

        while (nodeStack.size())
        {
            auto& item = nodeStack.top();

            size_t index = item.second++;
            if (index == 8)
            {
                nodeStack.pop();
                continue;
            }

            auto* node = item.first;
            auto& childNode = node->childNodes[index];
            
            bool intersectsNode = childNode->aabb.Intersects(newAabb);
            if (!intersectsNode)
            {
                continue;
            }

            const size_t currentLevel = nodeStack.size();
            
            if (currentLevel == m_maxLevels)
            {
                auto& objects = childNode->objects;
                if (!objects)
                {
                    objects = std::make_unique<Node::NodeObjects>();
                }
                objects->push_back(&object);

                return true;
            }

            if (!childNode)
            {
                //childNode = 
            }

            if (node->usedChildNodes != 0)
            {
                nodeStack.push({ childNode.get(), 0 });
                continue;
            }

           
            //nodeStack.push({ childNode, 0 });
        }

        return false;
    }

    template<typename ObjectT, typename CoordinateT>
    bool OctreeAABB<ObjectT, CoordinateT>::ForEachNode(std::function<void(AabbType)> callback)
    {
        using StackItem = std::pair<Node*, size_t>;
        std::stack<StackItem> nodeStack;
        nodeStack.push({ &m_rootNode, 0 });

        callback(m_rootNode.aabb);

        while (nodeStack.size())
        {
            auto& item = nodeStack.top();

            size_t index = item.second++;
            if (index == 8)
            {
                nodeStack.pop();
                continue;
            }

            Node* node = item.first;
            auto* childNode = node->childNodes[index].get();
            if (childNode)
            {
                callback(childNode->aabb);
                nodeStack.push({ childNode, 0 });
                continue;
            }
        }

        return false;
    }

    // Octree node implementations.
    template<typename ObjectT, typename CoordinateT>
    OctreeAABB<ObjectT, CoordinateT>::Node::Node(const AabbType& aabb) :
        aabb(aabb),
        childNodes{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, },
        usedChildNodes(0),
        objects{ nullptr }
    { }

}
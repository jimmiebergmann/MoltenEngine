/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef CURSE_CORE_RENDERER_MATERIAL_HPP
#define CURSE_CORE_RENDERER_MATERIAL_HPP

#include "Curse/Renderer/Material/MaterialNode.hpp"
#include <vector>
#include <set>
#include <string>

namespace Curse
{

    namespace Material
    {

        /**
        * @brief Material script class, used for generating shaders via virtual script.
        */
        class CURSE_API Script
        {

        public:

            /**
            * @brief Destructor.
            */
            ~Script();

            /**
            * @brief Create new constant node and append it to the material.
            *
            * @tparam T Constant node object to create.
            */
            template<typename T>
            ConstantNode<T>* CreateConstantNode(const T& value = PinDefault<T>::Value);

            /**
            * @brief Create new function node and append it to the material.
            *
            * @tparam T Function node object to create and data types for parameters.
            */
            template<typename Func>
            auto CreateFunctionNode();

            /**
            * @brief Create new operator node and append it to the material.
            *
            * @tparam T Operator node object to create.
            */
            template<typename T>
            OperatorNode<T>* CreateOperatorNode(const Operator op);

            /**
            * @brief Create new output node and append it to the material.
            *
            * @tparam T Output node object to create.
            */
            template<typename T>
            OutputNode<T>* CreateOutputNode();

            /**
            * @brief Create new varying node and append it to the material.
            *        Varying data is sent from the vertex or geometry shader.
            *
            * @tparam T Varying node object to create.
            */
            template<VaryingType T>
            VaryingNode<T>* CreateVaryingNode();

            /**
            * @brief Removes the node from the script, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(Node & node);

            /**
            * @brief Get all nodes of material.
            */
            std::vector<Node*> GetNodes();

            /**
            * @brief Get all nodes of material.
            */
            std::vector<const Node*> GetNodes() const;

            /**
            * @brief Get output nodes of material.
            */
            std::vector<Node*> GetOutputNodes();

            /**
            * @brief Get output nodes of material.
            */
            std::vector<const Node*> GetOutputNodes() const;

            /**
            * @brief Generate GLSL code from material.
            */
            std::string GenerateGlsl() const;

        private:

            std::set<Node*> m_allNodes; ///< Set of all nodes of any type.
            std::vector<Node*> m_varyingNodes;
            std::vector<Node*> m_outputNodes;

        };

    }

}

#include "MaterialScript.inl"

#endif
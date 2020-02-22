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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERNODE_HPP

#include "Curse/Renderer/Shader/ShaderPin.hpp"
#include <memory>
#include <array>

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Forward declarations.
        */
        class Script;
        class VertexScript;
        class FragmentScript;
        

        /**
        * @brief Enumerator of node types.
        */
        enum class NodeType : uint8_t
        {
            Constant,       ///< Local constant, only present in fragment shader.
            Function,       ///< Built-in shader function.
            Input,          ///< Input data from previous shader stage, or data from vertex buffer.
            Operator,       ///< Operator node in local space.
            Output,         ///< Output data for next shader stage, or data for framebuffer.
            Uniform,        ///< Uniform node, single object being sent runtime from client.
            VertexOutput    ///< Output data of vertex position, being outputted in the vertex shader stage.
        };


        /**
        * @brief shader script node base class.
        */
        class CURSE_API Node
        {

        public:

            /**
            * @brief Get parent shader script.
            */
            /**@{*/
            Script& GetScript();
            const Script& GetScript() const;
            /**@}*/

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const;

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0);
            virtual const Pin* GetInputPin(const size_t index = 0) const;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins();
            virtual std::vector<const Pin*> GetInputPins() const;
            /**@}*/

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0);
            virtual const Pin* GetOutputPin(const size_t index = 0) const;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins();
            virtual std::vector<const Pin*> GetOutputPins() const;
            /**@}*/

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const = 0;

        protected:

            Node(Script& script);
            Node(const Node&) = delete;
            Node(Node&&) = delete;
            virtual ~Node();

        private:

            Script& m_script; ///< Parent shader script.

            friend class VertexScript;
            friend class FragmentScript;

        }; 

    }

}

#endif
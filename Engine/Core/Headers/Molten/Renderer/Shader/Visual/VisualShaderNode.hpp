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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERNODE_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERNODE_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderPin.hpp"
#include <memory>
#include <array>

namespace Molten::Shader::Visual
{

    class Script;
    class VertexScript;
    class FragmentScript;
        

    /** Enumerator of node types. */
    enum class NodeType : uint8_t
    {
        Function,       ///< Built-in shader function.
        Operator,       ///< Operator node in local space.
        PushConstant,   ///< Push constant node, constants set by the client.
        Uniform,        ///< Uniform node, single object being sent runtime from client.

        Variable  ///< ???
    };


    /** Visual shader script node. This type is inherited by all script nodes.*/
    class MOLTEN_API Node
    {

    public:

        /** Get parent script of this node. */
        /**@{*/
        Script& GetScript();
        const Script& GetScript() const;
        /**@}*/

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const;

        /**  Get number of output pins.*/
        virtual size_t GetOutputPinCount() const;

        /**
         * Get input pin by index.
         *
         * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
         */
        /**@{*/
        virtual Pin* GetInputPin(const size_t index = 0);
        virtual const Pin* GetInputPin(const size_t index = 0) const;
        /**@}*/

        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetInputPins();
        virtual std::vector<const Pin*> GetInputPins() const;
        /**@}*/

        /**
         * Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        virtual Pin* GetOutputPin(const size_t index = 0);
        virtual const Pin* GetOutputPin(const size_t index = 0) const;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetOutputPins();
        virtual std::vector<const Pin*> GetOutputPins() const;
        /**@}*/

        /** Get type of node. */
        virtual NodeType GetType() const = 0;

    protected:

        Node(Script& script);
        virtual ~Node() = default;

        Node(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator =(const Node&) = delete;
        Node& operator =(Node&&) = delete;

    private:

        Script& m_script; ///< Parent shader script.

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#endif
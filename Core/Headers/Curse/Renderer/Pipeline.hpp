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

#ifndef CURSE_CORE_RENDERER_PIPELINE_HPP
#define CURSE_CORE_RENDERER_PIPELINE_HPP

#include "Curse/Renderer/Resource.hpp"
#include <vector>

namespace Curse
{

    class Shader;

    /**
    * @brief Pipeline base class.
    */
    class CURSE_API Pipeline
    {

    public:

        /**
        * @brief Enumerator of different vertex structes.
        */
        enum class Topology : uint8_t
        {
            PointList,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip
        };

        /**
        * @brief Enumerator of different polygon rendering modes.
        */
        enum class PolygonMode : uint8_t
        {
            Point,
            Line,
            Fill
        };

        /**
        * @brief Enumerator of front faces.
        */
        enum class FrontFace : uint8_t
        {
            Clockwise,
            Counterclockwise
        };

        /**
        * @brief Enumerator of culling modes
        */
        enum class CullMode : uint8_t
        {
            None,
            Front,
            Back,
            FrontAndBack
        };

        /**
        * @brief Enumerator of attribute data formats.
        */
        enum class AttributeFormat : uint8_t
        {
            R32_Float,
            R32_G32_Float,
            R32_G32_B32_Float,
            R32_G32_B32_A32_Float,
        };

        /**
        * @brief Vertex attribute information.
        */
        struct VertexAttribute
        {
            uint32_t location;
            uint32_t offset;
            AttributeFormat format;
        };

        /**
        * @brief Vertex binding information.
        */
        struct VertexBinding
        {
            uint32_t binding;
            uint32_t stride;
            std::vector<VertexAttribute> attributes;
        };

    protected:

        Pipeline() = default;
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        virtual ~Pipeline() = default;

        Resource resource;

    };

    /**
    * @brief Descriptor class of pipeline class.
    */
    class CURSE_API PipelineDescriptor
    {

    public:

        PipelineDescriptor() = default;
        
        Pipeline::Topology topology;
        Pipeline::PolygonMode polygonMode;
        Pipeline::FrontFace frontFace;
        Pipeline::CullMode cullMode;
        std::vector<Shader*> shaders;
        std::vector<Pipeline::VertexBinding> vertexBindings;
    };

}

#endif

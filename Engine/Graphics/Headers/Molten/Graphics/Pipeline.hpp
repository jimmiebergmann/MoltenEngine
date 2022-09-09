/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_GRAPHICS_PIPELINE_HPP
#define MOLTEN_GRAPHICS_PIPELINE_HPP

#include "Molten/Graphics/RenderResource.hpp"

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{
    class ShaderProgram;
    class RenderPass;

    
    /** Pipeline base class. */
    class MOLTEN_GRAPHICS_API Pipeline
    {

    public:

        /** Enumerator of blend functions. */
        enum class BlendFunction : uint8_t
        {
            Zero,
            One,
            SourceColor,
            SourceAlpha,
            DestinationColor,
            DestinationAlpha,
            OneMinusSourceColor,
            OneMinusSourceAlpha,
            OneMinusDestinationColor,
            OneMinusDestinationAlpha
        };

        /** Enumerator of blend operators. */
        enum class BlendOperator : uint8_t
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max
        };

        /** Enumerator of vertex structes. */
        enum class Topology : uint8_t
        {
            PointList,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip
        };

        /** Enumerator of polygon rendering modes. */
        enum class PolygonMode : uint8_t
        {
            Point,
            Line,
            Fill
        };

        /**  Enumerator of front faces. */
        enum class FrontFace : uint8_t
        {
            Clockwise,
            Counterclockwise
        };

        /** Enumerator of culling modes. */
        enum class CullMode : uint8_t
        {
            None,
            Front,
            Back,
            FrontAndBack
        };

        Pipeline() = default;
        virtual ~Pipeline() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator =(const Pipeline&) = delete;
        Pipeline& operator =(Pipeline&&) = delete;
        /**@}*/       

    };

    /** Descriptor of pipeline blending. */
    struct MOLTEN_GRAPHICS_API PipelineBlendingDescriptor
    {
        Pipeline::BlendOperator blendOperator = Pipeline::BlendOperator::Add;
        Pipeline::BlendFunction sourceColor = Pipeline::BlendFunction::SourceAlpha;
        Pipeline::BlendFunction sourceAlpha = Pipeline::BlendFunction::SourceAlpha;
        Pipeline::BlendFunction destinationColor = Pipeline::BlendFunction::OneMinusSourceAlpha;
        Pipeline::BlendFunction destinationAlpha = Pipeline::BlendFunction::OneMinusSourceAlpha;
    };

    /** Descriptor of pipeline. */
    struct MOLTEN_GRAPHICS_API PipelineDescriptor
    {
        Pipeline::Topology topology = Pipeline::Topology::TriangleList;
        Pipeline::PolygonMode polygonMode = Pipeline::PolygonMode::Fill;
        Pipeline::FrontFace frontFace = Pipeline::FrontFace::Clockwise;
        Pipeline::CullMode cullMode = Pipeline::CullMode::None;
        PipelineBlendingDescriptor blending = {};
        SharedRenderResource<RenderPass> renderPass = {};
        SharedRenderResource<ShaderProgram> shaderProgram = {};
    };

}

#endif

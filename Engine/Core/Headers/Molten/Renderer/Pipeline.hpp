/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_PIPELINE_HPP
#define MOLTEN_CORE_RENDERER_PIPELINE_HPP

#include "Molten/Renderer/ShaderProgram.hpp"

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{
    
    /** Pipeline base class. */
    class MOLTEN_API Pipeline
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
    struct MOLTEN_API PipelineBlendingDescriptor
    {
        PipelineBlendingDescriptor();
        PipelineBlendingDescriptor(
            const Pipeline::BlendOperator blendOperator,
            const Pipeline::BlendFunction sourceColor,
            const Pipeline::BlendFunction sourceAlpha,
            const Pipeline::BlendFunction destinationColor,
            const Pipeline::BlendFunction destinationAlpha);

        ~PipelineBlendingDescriptor() = default;

        PipelineBlendingDescriptor(const PipelineBlendingDescriptor&) = default;
        PipelineBlendingDescriptor(PipelineBlendingDescriptor&&) = default;
        PipelineBlendingDescriptor& operator =(const PipelineBlendingDescriptor&) = default;
        PipelineBlendingDescriptor& operator =(PipelineBlendingDescriptor&&) = default;

        Pipeline::BlendOperator blendOperator;
        Pipeline::BlendFunction sourceColor;
        Pipeline::BlendFunction sourceAlpha;
        Pipeline::BlendFunction destinationColor;
        Pipeline::BlendFunction destinationAlpha;
        
    };

    /** Descriptor of pipeline. */
    struct MOLTEN_API PipelineDescriptor
    {

        PipelineDescriptor();
        PipelineDescriptor(
            const Pipeline::Topology topology,
            const Pipeline::PolygonMode polygonMode,
            const Pipeline::FrontFace frontFace,
            const Pipeline::CullMode cullMode,
            const PipelineBlendingDescriptor blending,
            SharedRenderResource<ShaderProgram> shaderProgram);
        ~PipelineDescriptor() = default;

        PipelineDescriptor(const PipelineDescriptor&) = default;
        PipelineDescriptor(PipelineDescriptor&&) = default;
        PipelineDescriptor& operator =(const PipelineDescriptor&) = default;
        PipelineDescriptor& operator =(PipelineDescriptor&&) = default;
        
        Pipeline::Topology topology;
        Pipeline::PolygonMode polygonMode;
        Pipeline::FrontFace frontFace;
        Pipeline::CullMode cullMode;
        PipelineBlendingDescriptor blending;
        SharedRenderResource<ShaderProgram> shaderProgram;

    };

}

#endif

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


#include "Molten/Renderer/Pipeline.hpp"

namespace Molten
{

    // Pipeline blending descriptor.
    PipelineBlendingDescriptor::PipelineBlendingDescriptor() :
        blendOperator(Pipeline::BlendOperator::Add),
        sourceColor(Pipeline::BlendFunction::SourceAlpha),
        sourceAlpha(Pipeline::BlendFunction::SourceAlpha),
        destinationColor(Pipeline::BlendFunction::OneMinusSourceAlpha),
        destinationAlpha(Pipeline::BlendFunction::OneMinusSourceAlpha)
    {}

    PipelineBlendingDescriptor::PipelineBlendingDescriptor(
        const Pipeline::BlendOperator blendOperator,
        const Pipeline::BlendFunction sourceColor,
        const Pipeline::BlendFunction sourceAlpha,
        const Pipeline::BlendFunction destinationColor,
        const Pipeline::BlendFunction destinationAlpha
    ) :
        blendOperator(blendOperator),
        sourceColor(sourceColor),
        sourceAlpha(sourceAlpha),
        destinationColor(destinationColor),
        destinationAlpha(destinationAlpha)
    {}


    // Pipeline descriptor.
    PipelineDescriptor::PipelineDescriptor() :
        topology(Pipeline::Topology::TriangleList),
        polygonMode(Pipeline::PolygonMode::Fill),
        frontFace(Pipeline::FrontFace::Clockwise),
        cullMode(Pipeline::CullMode::None)
    { }

    PipelineDescriptor::PipelineDescriptor(
        const Pipeline::Topology topology,
        const Pipeline::PolygonMode polygonMode,
        const Pipeline::FrontFace frontFace,
        const Pipeline::CullMode cullMode,
        const PipelineBlendingDescriptor blending,
        SharedRenderResource<ShaderProgram> shaderProgram
    ) :
        topology(topology),
        polygonMode(polygonMode),
        frontFace(frontFace),
        cullMode(cullMode),
        blending(blending),
        shaderProgram(std::move(shaderProgram))
    { }

}
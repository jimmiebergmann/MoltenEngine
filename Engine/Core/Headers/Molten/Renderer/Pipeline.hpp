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

#ifndef MOLTEN_CORE_RENDERER_PIPELINE_HPP
#define MOLTEN_CORE_RENDERER_PIPELINE_HPP

#include "Molten/Renderer/Shader.hpp"
#include <vector>

namespace Molten::Shader
{
    class VertexStage;
    class FragmentStage;
}

namespace Molten
{
    
    /** Pipeline base class. */
    class MOLTEN_API Pipeline
    {

    public:

        /** Enumerator of different vertex structes. */
        enum class Topology : uint8_t
        {
            PointList,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip
        };

        /** Enumerator of different polygon rendering modes. */
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

    protected:

        Pipeline() = default;
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        virtual ~Pipeline() = default;

    };

    /** Descriptor class of pipeline class. */
    class MOLTEN_API PipelineDescriptor
    {

    public:

        PipelineDescriptor();
        
        Pipeline::Topology topology;
        Pipeline::PolygonMode polygonMode;
        Pipeline::FrontFace frontFace;
        Pipeline::CullMode cullMode;
        Shader::VertexStage* vertexStage;
        Shader::FragmentStage* fragmentStage;

    };

}

#endif

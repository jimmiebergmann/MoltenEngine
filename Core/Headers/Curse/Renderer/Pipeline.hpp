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

        using FlagType = uint32_t;

        /**
        * @brief Enumerator of different vertex structes.
        */
        enum class Topology
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
        enum class PolygonMode
        {
            Point,
            Line,
            Fill
        };

        /**
        * @brief Enumerator of front faces.
        */
        enum class FrontFace
        {
            Clockwise,
            Counterclockwise
        };

        /**
        * @brief Enumerator of culling modes
        */
        enum class CullMode
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

        Resource resource;

    };

    /**
    * @brief Descriptor class of pipeline class.
    */
    class CURSE_API PipelineDescriptor
    {

    public:

        PipelineDescriptor() = default;
       
        std::vector<Shader*> shaders;
        Pipeline::Topology topology;
        Pipeline::PolygonMode polygonMode;
        Pipeline::FrontFace frontFace;
        Pipeline::CullMode cullMode;
    };

}

#endif

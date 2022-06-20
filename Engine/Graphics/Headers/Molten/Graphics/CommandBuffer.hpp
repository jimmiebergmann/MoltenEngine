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

#ifndef MOLTEN_GRAPHICS_COMMANDBUFFER_HPP
#define MOLTEN_GRAPHICS_COMMANDBUFFER_HPP

#include "Molten/Graphics/Build.hpp"
#include <Molten/Math/Matrix.hpp>
#include <Molten/Math/Vector.hpp>

namespace Molten
{

    /** Forward declarations. */
    /**@{*/
    class DescriptorSet;
    class FramedDescriptorSet;
    class FramedUniformBuffer;
    class IndexBuffer;
    class Pipeline;
    class UniformBuffer;
    class VertexBuffer;
    /**@}*/


    /** Command buffer, used for recording render commands for render pass. */
    class MOLTEN_GRAPHICS_API CommandBuffer
    {

    public:

        CommandBuffer() = default;
        virtual ~CommandBuffer() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer(CommandBuffer&&) = delete;
        CommandBuffer& operator =(const CommandBuffer&) = delete;
        CommandBuffer& operator =(CommandBuffer&&) = delete;
        /**@}*/


        /** Bind descriptor set to draw queue. */
        virtual void BindDescriptorSet(DescriptorSet& descriptorSet) = 0;

        /** Bind framed descriptor set to draw queue. */
        virtual void BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet) = 0;

        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Pipeline& pipeline) = 0;


        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(VertexBuffer& vertexBuffer) = 0;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer) = 0;


        /** Push constant values to currently bound pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
         /**@{*/
        virtual void PushConstant(const uint32_t location, const bool value) = 0;
        virtual void PushConstant(const uint32_t location, const int32_t value) = 0;
        virtual void PushConstant(const uint32_t location, const float value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector2f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector3f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector4f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Matrix4x4f32& value) = 0;
        /**@}*/


        /** Update uniform buffer data. */
        virtual void UpdateUniformBuffer(UniformBuffer& uniformBuffer, const void* data, const size_t size, const size_t offset) = 0;

        /** Update uniform buffer data. */
        virtual void UpdateFramedUniformBuffer(FramedUniformBuffer& framedUniformBuffer, const void* data, const size_t size, const size_t offset) = 0;

    };

}

#endif

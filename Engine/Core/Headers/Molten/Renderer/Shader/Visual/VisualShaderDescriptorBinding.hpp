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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORBINDING_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORBINDING_HPP

#include "Molten/Types.hpp"

namespace Molten::Shader::Visual
{

    class Script;
    template<typename TBindingType> class DescriptorBinding;
    template<typename ... TAllowedBindingTypes> class DescriptorSet;

    enum class DescriptorBindingType : uint32_t
    {
        UniformBuffer,
        Sampler2D
    };

    class DescriptorBindingBase
    {

    public:

        /** Get id of this binding. */
        [[nodiscard]] virtual uint32_t GetId() const = 0;

    private:

        template<typename TBindingType>
        friend class DescriptorBinding;

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        /** Private destructor. */
        virtual ~DescriptorBindingBase() = default;

    };


    template<typename TBindingType>
    class DescriptorBinding : public DescriptorBindingBase
    {

        static_assert(std::is_base_of_v<Node, TBindingType>, "Provided TBindingType must inherit from Node.");

    public:

        /** Constructor. */
        DescriptorBinding(Script& script, const uint32_t id);

        /** Get id of this binding. */
        [[nodiscard]] uint32_t GetId() const override;

    private:

        template<typename ... TAllowedBindingTypes> 
        friend class DescriptorSet;

        /** Private destructor. */
        ~DescriptorBinding() = default;

        DescriptorBinding(const DescriptorBinding&) = delete;
        DescriptorBinding(DescriptorBinding&&) = delete;
        DescriptorBinding& operator =(const DescriptorBinding&) = delete;
        DescriptorBinding& operator =(DescriptorBinding&&) = delete;

        uint32_t m_id;

    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderDescriptorBinding.inl"

#endif
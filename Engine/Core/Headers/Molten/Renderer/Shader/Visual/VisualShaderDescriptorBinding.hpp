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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORBINDING_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORBINDING_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderUniformBuffer.hpp"
#include "Molten/Renderer/DescriptorSet.hpp"

namespace Molten::Shader::Visual
{

    /** Forward declarations. */
    class DescriptorSetBase;
    template<typename ... TAllowedBindingTypes> class DescriptorSet;
    template<typename TBindingType, typename ... TAllowedDataTypes> class DescriptorBinding;
    

    /** Base class for Descriptor binding. */
    class MOLTEN_API DescriptorBindingBase : public Node
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBindingBase(const DescriptorBindingBase&) = delete;
        DescriptorBindingBase(DescriptorBindingBase&&) = delete;
        DescriptorBindingBase& operator = (const DescriptorBindingBase&) = delete;
        DescriptorBindingBase& operator = (DescriptorBindingBase&&) = delete;
        /**@}*/

        /** Get parent set. */
        [[nodiscard]] DescriptorSetBase& GetSet();
        [[nodiscard]] const DescriptorSetBase& GetSet() const;

        /** Get type of node. */
        [[nodiscard]] NodeType GetType() const override;

        /** Get id of this binding. */
        [[nodiscard]] virtual uint32_t GetId() const = 0;

        /** Get type of binding. */
        [[nodiscard]] virtual DescriptorBindingType GetBindingType() const = 0;

    protected:

        template<typename TBindingType, typename ... TAllowedDataTypes>
        friend class DescriptorBinding;

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        DescriptorBindingBase(
            Script& script,
            DescriptorSetBase& set);
        ~DescriptorBindingBase() override = default;

        DescriptorSetBase& m_setBase;

    };

    /** Descriptor binding class template. This template should be template specialized. */
    template<typename TBindingType, typename ... TAllowedDataTypes>
    class DescriptorBinding;
    

    /** Helper class template for samplers.*/
    template<typename TOutputType>
    class DescriptorBindingSamplerBase : public DescriptorBindingBase
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBindingSamplerBase(const DescriptorBindingSamplerBase&) = delete;
        DescriptorBindingSamplerBase(DescriptorBindingSamplerBase&&) = delete;
        DescriptorBindingSamplerBase& operator =(const DescriptorBindingSamplerBase&) = delete;
        DescriptorBindingSamplerBase& operator =(DescriptorBindingSamplerBase&&) = delete;
        /**@}*/

        /** Get output pin of sampler binding as reference. */
        /**@{*/
        [[nodiscard]] OutputPin<TOutputType>& GetOutput();
        [[nodiscard]] const OutputPin<TOutputType>& GetOutput() const;
        /**@}*/

        /** Get id of this binding. */
        [[nodiscard]] uint32_t GetId() const override;

        /** Get type of binding. */
        [[nodiscard]] DescriptorBindingType GetBindingType() const override;

        [[nodiscard]] size_t GetOutputPinCount() const override;

        [[nodiscard]] Pin* GetOutputPin(const size_t index = 0) override;
        [[nodiscard]] const Pin* GetOutputPin(const size_t index = 0) const override;

        [[nodiscard]] std::vector<Pin*> GetOutputPins() override;
        [[nodiscard]] std::vector<const Pin*> GetOutputPins() const override;

    protected:

        DescriptorBindingSamplerBase(
            Script& script,
            DescriptorSetBase& set,
            const uint32_t id);

        ~DescriptorBindingSamplerBase() override = default;

        uint32_t m_id;
        OutputPin<TOutputType> m_output;

    };


    /** Descriptor binding of sampler 1D type. */
    template<>
    class DescriptorBinding<Sampler1D> : public DescriptorBindingSamplerBase<Sampler1D>
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBinding(const DescriptorBinding&) = delete;
        DescriptorBinding(DescriptorBinding&&) = delete;
        DescriptorBinding& operator =(const DescriptorBinding&) = delete;
        DescriptorBinding& operator =(DescriptorBinding&&) = delete;
        /**@}*/

    protected:

        template<typename ... TAllowedBindingTypes> 
        friend class DescriptorSet;

        DescriptorBinding(
            Script& script,
            DescriptorSetBase& set,
            const uint32_t id);

        ~DescriptorBinding() override = default;

    };

    /** Descriptor binding of sampler 2D type. */
    template<>
    class DescriptorBinding<Sampler2D> : public DescriptorBindingSamplerBase<Sampler2D>
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBinding(const DescriptorBinding&) = delete;
        DescriptorBinding(DescriptorBinding&&) = delete;
        DescriptorBinding& operator =(const DescriptorBinding&) = delete;
        DescriptorBinding& operator =(DescriptorBinding&&) = delete;
        /**@}*/

    protected:

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        DescriptorBinding(
            Script& script,
            DescriptorSetBase& set,
            const uint32_t id);

        ~DescriptorBinding() override = default;

    };

    /** Descriptor binding of sampler 3D type. */
    template<>
    class DescriptorBinding<Sampler3D> : public DescriptorBindingSamplerBase<Sampler3D>
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBinding(const DescriptorBinding&) = delete;
        DescriptorBinding(DescriptorBinding&&) = delete;
        DescriptorBinding& operator =(const DescriptorBinding&) = delete;
        DescriptorBinding& operator =(DescriptorBinding&&) = delete;
        /**@}*/

    protected:

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        DescriptorBinding(
            Script& script,
            DescriptorSetBase& set,
            const uint32_t id);

        ~DescriptorBinding() override = default;

    };

    /** Descriptor binding of uniform buffer type. */
    template<typename ... TAllowedDataTypes>
    class DescriptorBinding<UniformBuffer<TAllowedDataTypes...>> : public DescriptorBindingBase
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        DescriptorBinding(const DescriptorBinding&) = delete;
        DescriptorBinding(DescriptorBinding&&) = delete;
        DescriptorBinding& operator =(const DescriptorBinding&) = delete;
        DescriptorBinding& operator =(DescriptorBinding&&) = delete;
        /**@}*/

        /** Get id of this binding. */
        [[nodiscard]] uint32_t GetId() const override;

        /** Get type of binding. */
        [[nodiscard]] DescriptorBindingType GetBindingType() const override;

        /**  Get number of output pins.*/
        [[nodiscard]] size_t GetOutputPinCount() const override;

        /** Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        [[nodiscard]] Pin* GetOutputPin(const size_t index = 0) override;
        [[nodiscard]] const Pin* GetOutputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        [[nodiscard]] std::vector<Pin*> GetOutputPins() override;
        [[nodiscard]] std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/

        /** Append new pin to this structure. */
        template<typename TDataType>
        [[nodiscard]] OutputPin<TDataType>& AddPin();

        /** Remove and destroy all pins. Accessing any removed pin is undefined behaviour. */
        void RemovePin(const size_t index);

        /** Remove and destroy all pins. Accessing any removed pinr is undefined behaviour. */
        void RemoveAllPins();

        /** Get number of pins in this structure. */
        [[nodiscard]] size_t GetPinCount() const;

        /** Get sum of pin data member sizes in bytes. Kind of like sizeof(...). */
        [[nodiscard]] size_t GetSizeOf() const;

    private:

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        DescriptorBinding(
            Script& script,
            DescriptorSetBase& set,
            const uint32_t id);

        ~DescriptorBinding() override;

        template<typename T>
        std::vector<T*> CreatePinVector() const;

        struct PinWrapper
        {
            Pin* pin;
            size_t dataTypeSize;
        };

        using PinWrapperContainer = std::vector<PinWrapper>;

        uint32_t m_id;
        PinWrapperContainer m_pinWrappers; ///< Container of data pins.
        size_t m_sizeOf; ///< Total size of this structure, in bytes.

    };


    /** Descriptor binding traits. */
    template<typename TOutputType>
    struct DescriptorBindingSamplerTraits
    {
        //static constexpr DescriptorBindingType bindingType = ? ;
    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderDescriptorBinding.inl"

#endif
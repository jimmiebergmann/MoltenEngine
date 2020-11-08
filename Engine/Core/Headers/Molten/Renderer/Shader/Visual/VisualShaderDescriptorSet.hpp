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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORSET_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERDESCRIPTORSET_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderDescriptorBinding.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderUniformBuffer.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderSampler.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"
#include <set>
#include <vector>

namespace Molten::Shader::Visual
{

    class Script;
    template<typename ... TAllowedBindingTypes> class DescriptorSet;
    template<typename ... TAllowedBindingTypes> class DescriptorSets;


    class DescriptorSetBase
    {

    public:

        /** Get parent script of descriptor set. */
        /**@{*/
        [[nodiscard]] virtual Script& GetScript() = 0;
        [[nodiscard]] virtual const Script& GetScript() const = 0;
        /**@}*/

        /** Get id of this set. */
        [[nodiscard]] virtual uint32_t GetId() const = 0;

        /** Get descriptor binding by index. Returns nullptr if index >= GetBindingCount().*/
        /**@{*/
        [[nodiscard]] virtual DescriptorBindingBase* GetBindingBase(const size_t index) = 0;
        [[nodiscard]] virtual const DescriptorBindingBase* GetBindingBase(const size_t index) const = 0;
        /**@}*/

        /** Get number of descriptor bindings in this container. */
        [[nodiscard]] virtual size_t GetBindingCount() const = 0;

    private:

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSet;

        virtual ~DescriptorSetBase() = default;

    };


    template<typename ... TAllowedBindingTypes>
    class DescriptorSet : public DescriptorSetBase
    {

    public:

        using Container = std::vector<DescriptorBindingBase*>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;


        /** Constructor. */
        DescriptorSet(Script& script, const uint32_t id);


        /** Iterators. */
        /**@{*/
        Iterator begin();
        ConstIterator begin() const;

        Iterator end();
        ConstIterator end() const;
        /**@}*/


        /** Get parent script of descriptor set. */
        /**@{*/
        [[nodiscard]] Script& GetScript() override;
        [[nodiscard]] const Script& GetScript() const override;
        /**@}*/

        /** Get id of this set. */
        [[nodiscard]] uint32_t GetId() const override;


        /** Add new binding of type TBindingType to this container. */
        template<typename TBindingType>
        DescriptorBinding<TBindingType>* AddBinding(const uint32_t id);

        /**
        * Remove and destroy descriptor binding by index or iterator.
        * Accessing removed descriptor bindings is undefined behaviour.
        */
        /**@{*/
        void RemoveBinding(const size_t index);
        void RemoveBinding(Iterator it);
        void RemoveBinding(ConstIterator it);
        /**@}*/

        /** Remove all descriptor bindings in this container. */
        void RemoveAllBindings();

        /** Get descriptor binding by index. Returns nullptr if index >= GetBindingCount().*/
        /**@{*/
        template<typename TBindingType>
        [[nodiscard]] DescriptorBinding<TBindingType>* GetBinding(const size_t index);
        template<typename TBindingType>
        [[nodiscard]] const DescriptorBinding<TBindingType>* GetBinding(const size_t index) const;
        [[nodiscard]] DescriptorBindingBase* GetBindingBase(const size_t index) override;
        [[nodiscard]] const DescriptorBindingBase* GetBindingBase(const size_t index) const override;
        /**@}*/

        /** Get number of descriptor bindings in this container. */
        [[nodiscard]] size_t GetBindingCount() const override;

    private:

        template<typename ... TAllowedBindingTypes>
        friend class DescriptorSets;

        /** Private destructor. */
        ~DescriptorSet();

        DescriptorSet(const DescriptorSet&) = delete;
        DescriptorSet(DescriptorSet&&) = delete;
        DescriptorSet& operator =(const DescriptorSet&) = delete;
        DescriptorSet& operator =(DescriptorSet&&) = delete;      

        Script& m_script;
        uint32_t m_id;
        Container m_bindings;
        std::set<uint32_t> m_usedBindingIds;

    };


    /** Descriptor set contrainer base class. */
    class MOLTEN_API DescriptorSetsBase
    {

    public:

        virtual ~DescriptorSetsBase() = default;


        /** Get descriptor set by index. Returns nullptr if index >= GetSetCount(). */
        /**@{*/
        [[nodiscard]] virtual DescriptorSetBase* GetSetBase(const size_t index) = 0;
        [[nodiscard]] virtual const DescriptorSetBase* GetSetBase(const size_t index) const = 0;
        /**@}*/

        /** Get number of descriptor sets in this container. */
        [[nodiscard]] virtual size_t GetSetCount() const = 0;

    private:

    };


    /** Descriptor set contrainer class. */
    template<typename ... TAllowedBindingTypes>
    class DescriptorSets : public DescriptorSetsBase
    {

    public:

        using Container = std::vector<DescriptorSet<TAllowedBindingTypes...>*>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;
        using DescriptorSetType = DescriptorSet<TAllowedBindingTypes...>;


        /** Constructor. */
        explicit DescriptorSets(Script& m_script);

        /** Destructor. */
        ~DescriptorSets();
                

        /** Iterators. */
        /**@{*/
        Iterator begin();
        ConstIterator begin() const;

        Iterator end();
        ConstIterator end() const;
        /**@}*/


        /** Add descriptor set to this container.
         *  The passed is can be any number, but it should be wisely chosen.
         *  A smaller id should be used for frequently updated sets, and more static sets should be higher numbers.
         *  An exemplate of id's could be 0 for transformations, 1 for camera view and 2 for samplers.
         *
         * @return Pointer to added descriptor set, nullptr if a set with provided id already exists in this container.
         */
        DescriptorSetType* AddSet(const uint32_t id);

        /**
         * Remove and destroy descriptor set by index or iterator.
         * Accessing removed descriptor sets is undefined behaviour.
         */
        /**@{*/
        void RemoveSet(const size_t index);
        void RemoveSet(Iterator it);
        void RemoveSet(ConstIterator it);
        /**@}*/

        /** Remove all descriptor sets in this container. */
        void RemoveAllSets();

        /** Get descriptor set by index. Returns nullptr if index >= GetSetCount(). */
        /**@{*/
        [[nodiscard]] DescriptorSetType* GetSet(const size_t index);
        [[nodiscard]] const DescriptorSetType* GetSet(const size_t index) const;
        [[nodiscard]] DescriptorSetBase* GetSetBase(const size_t index) override;
        [[nodiscard]] const DescriptorSetBase* GetSetBase(const size_t index) const override;
        /**@}*/

        /** Get number of descriptor sets in this container. */
        [[nodiscard]] size_t GetSetCount() const override;

    private:

        DescriptorSets(const DescriptorSets&) = delete;
        DescriptorSets(DescriptorSets&&) = delete;
        DescriptorSets& operator =(const DescriptorSets&) = delete;
        DescriptorSets& operator =(DescriptorSets&&) = delete;

        Script& m_script;
        Container m_sets;
        std::set<uint32_t> m_usedSetIds;

    };


    using FragmentDescriptorSets = 
        DescriptorSets<
            DescriptorBinding<FragmentUniformBuffer>,
            DescriptorBinding<Sampler1D>,
            DescriptorBinding<Sampler2D>,
            DescriptorBinding<Sampler3D>
        >;

    using VertexDescriptorSets =
        DescriptorSets<
            DescriptorBinding<VertexUniformBuffer>
        >;

}

#include "Molten/Renderer/Shader/Visual/VisualShaderDescriptorSet.inl"

#endif
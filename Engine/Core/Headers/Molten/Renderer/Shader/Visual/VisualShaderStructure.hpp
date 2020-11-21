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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSTRUCTURE_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSTRUCTURE_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"
#include <vector>

namespace Molten::Shader::Visual
{

    /** Structure member base class. */
    template<typename TMetaData>
    class StructureMemberBase : public TMetaData
    {

    public:

        /** Constructor. */
        template<typename ... TArgs>
        StructureMemberBase(TArgs ... args);

        /** Virtual destructor. */
        virtual ~StructureMemberBase() = default;

        /** Get pin associated with this member. */
        /**@{*/
        virtual Pin& GetPin() = 0;
        virtual const Pin& GetPin() const = 0;
        /**@}*/

    };

    /** Special case template specialization of structure member base, where no meta data declared. */
    template<>
    class StructureMemberBase<void>
    {

    public:

        /** Virtual destructor. */
        virtual ~StructureMemberBase() = default;

        /** Get pin associated with this member. */
        /**@{*/
        virtual Pin& GetPin() = 0;
        virtual const Pin& GetPin() const = 0;
        /**@}*/

    };

    /** Structure member class. */
    template<typename TPinType, typename TMetaData>
    class StructureMember : public StructureMemberBase<TMetaData>, public TPinType
    {

    public:

        /** Constructor. */
        template<typename ... TArgs>
        StructureMember(Node& node, TArgs ... args);

        /** Get pin associated with this member. */
        /**@{*/
        Pin& GetPin() override;
        const Pin& GetPin() const override;
        /**@}*/

    };


    /** Structure base class. */
    template<typename TMetaData>
    class StructureBase : public Node
    {

    public:

        using MemberBaseType = StructureMemberBase<TMetaData>;

        /** Constructor. */
        StructureBase(Script& script);

        /** Virtual destructor. */
        virtual ~StructureBase() = default;


        /** Get member by index. */
        /**@{*/
        virtual MemberBaseType* GetMemberBase(const size_t index) = 0;
        virtual const MemberBaseType* GetMemberBase(const size_t index) const = 0;
        /**@}*/

        /** Get number of pins in this structure. */
        virtual size_t GetMemberCount() const = 0;

        /** Get sum of pin data member sizes in bytes. Kind of like sizeof(...). */
        virtual size_t GetSizeOf() const = 0;

    };


    /** Data structure container for visual shader scripts.
     *  This container can be used for interface blocks of unifom buffers, vertex data or push constants.
     *  Allowed types for template parameter TPinType are InputPin or OutputPin.
     *
     * @see Script
     */
    template<NodeType VNodeType, template<typename> typename TPinType, typename TMetaData, typename ... TAllowedDataTypes>
    class Structure : public StructureBase<TMetaData>
    {

        static_assert(PinTraits<TPinType>::isInputPin || PinTraits<TPinType>::isOutputPin,
            "Provided TPinType is not of type InputPin or OutputPin");

    public:

        using Base = StructureBase<TMetaData>;
        template<typename TDataType>
        using PinType = TPinType<TDataType>;
        template<typename TDataType>
        using MemberType = StructureMember<PinType<TDataType>, TMetaData>;
        using MemberBaseType = StructureMemberBase<TMetaData>;
        using Container = std::vector<MemberBaseType*>;
        using ConstContainer = std::vector<const MemberBaseType*>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;

        /** Constructor. */
        explicit Structure(Script& script);

        /** Destructor. */
        virtual ~Structure();


        /** Structure iterators. */
        /**@{*/
        Iterator begin();
        ConstIterator begin() const;

        Iterator end();
        ConstIterator end() const;
        /**@}*/


        /** Get type of node. */
        NodeType GetType() const override;

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const override;

        /**  Get number of output pins.*/
        virtual size_t GetOutputPinCount() const override;

        /** Get input pin of member by index.
         *
         * @return Pointer of input pin of member at given index, nullptr if index is >= GetInputPinCount().
         */
        /**@{*/
        virtual Pin* GetInputPin(const size_t index) override;
        virtual const Pin* GetInputPin(const size_t index) const override;
        
        template<typename TDataType>
        InputPin<TDataType>* GetInputPin(const size_t index);

        template<typename TDataType>
        const InputPin<TDataType>* GetInputPin(const size_t index) const;
        /**@}*/
        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;
        /**@}*/

        /** Get output pin of member by index. 
         *
         * @return Pointer of output pin of member at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        virtual Pin* GetOutputPin(const size_t index) override;
        virtual const Pin* GetOutputPin(const size_t index) const override;

        template<typename TDataType>
        OutputPin<TDataType>* GetOutputPin(const size_t index);

        template<typename TDataType>
        const OutputPin<TDataType>* GetOutputPin(const size_t index) const;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/

        /** Append new pin to this structure. */
        template<typename TDataType, typename ... TArgs>
        MemberType<TDataType>& AddMember(TArgs ... args);

        /** Remove and destroy all pins. Accessing any removed pin is undefined behaviour. */
        void RemoveMember(const size_t index);
        void RemoveMember(const Iterator it);
        void RemoveMember(const ConstIterator it);

        /** Remove and destroy all pins. Accessing any removed pinr is undefined behaviour. */
        void RemoveAllMembers();

        /** Get member by index. */
        /**@{*/
        MemberBaseType* GetMemberBase(const size_t index) override;
        const MemberBaseType* GetMemberBase(const size_t index) const override;
        /**@}*/

        /** Get number of pins in this structure. */
        size_t GetMemberCount() const override;

        /** Get sum of pin data member sizes in bytes. Kind of like sizeof(...). */
        size_t GetSizeOf() const override;

        /** Compare pin types with another structure. Returns true if layout is the same, else false. */
        template<NodeType VOtherNodeType, template<typename> typename TOtherPinType, typename TOtherMetaData, typename ... TOtherAllowedDataTypes>
        bool CompareStructure(const Structure<VOtherNodeType, TOtherPinType, TOtherMetaData, TOtherAllowedDataTypes...>& other) const;

    private:

        template<NodeType VOtherNodeType, template<typename> typename TOtherPinType, typename TOtherMetaData, typename ... TOtherAllowedDataTypes>
        friend class Structure;

        /** Copy and move operations are not allowed. */
        /**@{*/
        Structure(const Structure& copy) = delete;
        Structure(const Structure&& move) = delete;
        Structure& operator =(const Structure& copy) = delete;
        Structure& operator =(const Structure&& move) = delete;
        /**@*/

        /** Helper template function for creating vector of pins.*/
        template<typename T>
        std::vector<T*> CreatePinVector() const;

        Script& m_script; ///< parent script.
        Container m_members; ///< Container of data pins.
        size_t m_sizeOf; ///< Total size of this structure, in bytes.

    };


    /** Meta data class for structures, only containing an constant id. */
    template<typename T>
    class MetaDataConstantId
    {

    public:

        using Type = T;

        /** Constructor. */
        MetaDataConstantId(const Type id);

        /** Get id. */
        Type GetId() const;

    private:

        const Type m_id;

    };


    template<NodeType VNodeType, typename TMetaData, typename ... TAllowedDataTypes>
    using InputStructure = Structure<VNodeType, InputPin, TMetaData, TAllowedDataTypes...>;

    template<NodeType VNodeType, typename TMetaData, typename ... TAllowedDataTypes>
    using OutputStructure = Structure<VNodeType, OutputPin, TMetaData, TAllowedDataTypes...>;

}

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.inl"

#endif
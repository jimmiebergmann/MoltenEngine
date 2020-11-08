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

#include "Molten/Renderer/Shader/Visual/VisualShaderVariable.hpp"
#include "Molten/Utility/Template.hpp"
#include <vector>

namespace Molten::Shader::Visual
{

    /** Data structure container for visual shader scripts.
     *  This container can be used for interface blocks of unifom buffers, vertex data or push constants.
     *  Allowed types for template parameter TPinType are InputPin or OutputPin.
     *
     * @see Script
     */
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    class Structure : public Node
    {

        static_assert(PinTraits<TPinType>::isInputPin || PinTraits<TPinType>::isOutputPin,
            "Provided TPinType is not of type InputPin or OutputPin");

    public:

        using Base = Node;
        template<typename TDataType>
        using PinType = TPinType<TDataType>;
        using Container = std::vector<Pin*>;
        using ConstContainer = std::vector<const Pin*>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;

        /** Constructor. */
        explicit Structure(Script& script);
        virtual ~Structure();


        /** Structure iterators. */
        /**@{*/
        typename Iterator begin();
        typename ConstIterator begin() const;

        typename Iterator end();
        typename ConstIterator end() const;
        /**@}*/


        /** Get type of node. */
        virtual NodeType GetType() const = 0;

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const;

        /**  Get number of output pins.*/
        virtual size_t GetOutputPinCount() const;

        /** Get input pin by index.
         *
         * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
         */
        /**@{*/
        virtual Pin* GetInputPin(const size_t index = 0) override;
        virtual const Pin* GetInputPin(const size_t index = 0) const override;
        
        template<typename TDataType>
        InputPin<TDataType>* GetInputPin(const size_t index = 0);

        template<typename TDataType>
        const InputPin<TDataType>* GetInputPin(const size_t index = 0) const;
        /**@}*/
        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;
        /**@}*/

        /** Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        virtual Pin* GetOutputPin(const size_t index = 0) override;
        virtual const Pin* GetOutputPin(const size_t index = 0) const override;

        template<typename TDataType>
        OutputPin<TDataType>* GetOutputPin(const size_t index = 0);

        template<typename TDataType>
        const OutputPin<TDataType>* GetOutputPin(const size_t index = 0) const;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/

        /** Append new pin to this structure. */
        template<typename TDataType>
        PinType<TDataType>& AddPin();

        /** Remove and destroy all pins. Accessing any removed pin is undefined behaviour. */
        void RemovePin(const size_t index);
        void RemovePin(const Iterator it);
        void RemovePin(const ConstIterator it);

        /** Remove and destroy all pins. Accessing any removed pinr is undefined behaviour. */
        void RemoveAllPins();

        /** Get number of pins in this structure. */
        size_t GetPinCount() const;

        /** Get sum of pin data member sizes in bytes. Kind of like sizeof(...). */
        size_t GetSizeOf() const;

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        Structure(const Structure& copy) = delete;
        Structure(const Structure&& move) = delete;
        Structure& operator =(const Structure& copy) = delete;
        Structure& operator =(const Structure&& move) = delete;
        /**@*/

        template<typename T>
        std::vector<T*> CreatePinVector() const;

        struct PinWrapper
        { 
            Pin* pin;
            size_t dataTypeSize;
        };

        using PinWrapperContainer = std::vector<PinWrapper>;

        Script& m_script; ///< parent script.
        PinWrapperContainer m_pinWrappers; ///< Container of data pins.
        size_t m_sizeOf; ///< Total size of this structure, in bytes.

    };


    /** Helper template for creating a strucutre with given NodeType.
     *
     * @see Structure
     */
    template<NodeType TypeOfNode, template<typename> typename TPinType, typename ... TAllowedDataTypes>
    class StructureWithNodeType : public Structure<TPinType, TAllowedDataTypes...>
    {

    public:

        /** Constructor. */
        explicit StructureWithNodeType(Script& script);

        /** Destructor. */
        virtual ~StructureWithNodeType() = default;


        /** Get type of node. */
        NodeType GetType() const override;

    };

    template<typename ... TAllowedDataTypes>
    using InputStructure = Structure<InputPin, TAllowedDataTypes...>;

    template<typename ... TAllowedDataTypes>
    using OutputStructure = Structure<OutputPin, TAllowedDataTypes...>;

    template<NodeType TypeOfNode, typename ... TAllowedDataTypes>
    using InputStructureWithNodeType = StructureWithNodeType<TypeOfNode, InputPin, TAllowedDataTypes...>;

    template<NodeType TypeOfNode, typename ... TAllowedDataTypes>
    using OutputStructureWithNodeType = StructureWithNodeType<TypeOfNode, OutputPin, TAllowedDataTypes...>;

}

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.inl"

#endif
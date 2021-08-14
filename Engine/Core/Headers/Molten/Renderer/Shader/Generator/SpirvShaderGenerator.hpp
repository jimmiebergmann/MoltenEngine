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

#ifndef MOLTEN_CORE_RENDERER_SHADER_GENERATOR_SPIRVSHADERGENERATOR_HPP
#define MOLTEN_CORE_RENDERER_SHADER_GENERATOR_SPIRVSHADERGENERATOR_HPP

#include "Molten/Core.hpp"
#include "Molten/Renderer/Shader.hpp"
#include "Molten/Renderer/Shader/Spirv/SpirvModule.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <tuple>

/** Forward declarations. */
/**@{*/
namespace Molten
{
    class Logger;
}
namespace Molten::Shader::Visual
{
    class Pin;
    class Node;
    class Script;
}
/**@}*/


namespace Molten::Shader
{

    template<typename T, size_t VDimensions>
    struct SpirvGeneratorConstantVectorComparer;

    template<typename T>
    struct SpirvGeneratorConstantVectorComparer<T, 2>
    {
        bool operator()(const Vector2<T>& lhs, const Vector2<T>& rhs) const
        {
            return lhs.x < rhs.x || lhs.y < rhs.y;
        }
    };
    template<typename T>
    struct SpirvGeneratorConstantVectorComparer<T, 3>
    {
        bool operator()(const Vector3<T>& lhs, const Vector3<T>& rhs) const
        {
            return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z;
        }
    };
    template<typename T>
    struct SpirvGeneratorConstantVectorComparer<T, 4>
    {
        bool operator()(const Vector4<T>& lhs, const Vector4<T>& rhs) const
        {
            return lhs.x < rhs.x || lhs.y < rhs.y || lhs.z < rhs.z || lhs.w < rhs.w;
        }
    };


    /** No in use yet. */
    struct MOLTEN_API SpirvTemplate
    {
        
    };


    /** SPIR-V code generator from visual script. */
    class MOLTEN_API SpirvGenerator
    {

    public:

        using Template = SpirvTemplate;

        /** No in use yet. */
        static bool CreateTemplate(
            Template& glslTemplate,
            const std::vector<Visual::Script*>& scripts,
            Logger* logger);

        explicit SpirvGenerator(Logger* logger = nullptr);

        SpirvGenerator(const SpirvGenerator&) = delete;
        SpirvGenerator(SpirvGenerator&&) = delete;
        SpirvGenerator& operator = (const SpirvGenerator&) = delete;
        SpirvGenerator& operator = (SpirvGenerator&&) = delete;

        [[nodiscard]] Spirv::Words Generate(
            const Visual::Script& script,
            const Template* spirvTemplate = nullptr,
            const bool includeDebugSymbols = false);

    private:

        struct GeneratorNode;
        struct GeneratorOutputPin;


        // Data type.
        struct DataTypeDefinition
        {
            DataTypeDefinition(
                const VariableDataType dataType,
                const Spirv::Id typeId);

            VariableDataType dataType;
            Spirv::Id typeId;
        };
        using DataTypeDefinitionPointer = std::shared_ptr<DataTypeDefinition>;

        // Data type pointer.
        struct DataTypePointerDefinition
        {
            DataTypePointerDefinition(
                DataTypeDefinitionPointer dataTypeDefinition,
                const Spirv::Id pointerId);

            DataTypeDefinitionPointer dataTypeDefinition;
            Spirv::Id pointerId;
        };
        using DataTypePointerDefinitionPointer = std::shared_ptr<DataTypePointerDefinition>;

        // Constant
        template<typename T>
        struct Constant
        {
            Constant(
                const Spirv::Id id,
                DataTypeDefinitionPointer dataTypeDefinition,
                const T value);

            Spirv::Id id;
            DataTypeDefinitionPointer dataTypeDefinition;
            const T value;
        };

        template<typename T>
        using ConstantPointer = std::shared_ptr<Constant<T>>;

        // Push constant structures.
        struct PushConstantMember
        {
            Spirv::Word index;
            Spirv::Word offset;
            DataTypeDefinitionPointer dataTypeDefinition;
        };

        struct PushConstantStruct
        {
            Spirv::Id structTypeId;
            Spirv::Id structTypePointerId;
            Spirv::Id id;
            std::vector<PushConstantMember> members;
        };
        using PushConstantStructPointer = std::shared_ptr<PushConstantStruct>;

        // Input structure.
        struct InputOutputMember // TODO: In need anymore?
        {
            DataTypePointerDefinitionPointer dataTypePointerDefinition;
            Spirv::Id id;
            Spirv::Word location;
        };

        struct InputOutputStruct // TODO: In need anymore?
        {
            std::vector<InputOutputMember> members;
        };
        using InputOutputStructPointer = std::shared_ptr<InputOutputStruct>;

        // Uniform buffer.
        struct UniformBufferMember // TODO: In need anymore?
        {
            Spirv::Word index;
            Spirv::Word offset;
            DataTypeDefinitionPointer dataTypeDefinition;
            Spirv::Id pointerId;
            const Visual::Pin* pin;
        };
        using UniformBufferMemberPointer = std::shared_ptr<UniformBufferMember>;

        struct UniformBuffer // TODO: In need anymore?
        {
            Spirv::Id structTypeId;
            Spirv::Id structTypePointerId;
            Spirv::Id id;
            
            Spirv::Word setIndex;
            Spirv::Word bindingIndex;         

            std::vector<UniformBufferMemberPointer> members;
            std::map<const Visual::Pin*, UniformBufferMemberPointer> memberMap;
        };
        using UniformBufferPointer = std::shared_ptr<UniformBuffer>;
        using UniformBufferPointers = std::vector<UniformBufferPointer>;

        // Data type storage.
        struct DataTypeStorage
        {
            struct DataTypePair
            {
                VariableDataType dataType;
                DataTypeDefinitionPointer dataTypeDefintion;
            };

            DataTypeDefinitionPointer Find(const VariableDataType dataType);

            using DataTypePairs = std::vector<DataTypePair>;
            DataTypePairs dataTypes;
        };

        // Data type pointer storage.
        struct DataTypePointerStorage
        {
            using DataTypePointerMap = std::map<VariableDataType, DataTypePointerDefinitionPointer>;
            DataTypePointerMap dataTypePointers;
        };

        // Image type storage.
        struct ImageTypeStorage
        {
            struct ImageType
            {
                size_t dimensions;
                Spirv::Word typeId;
            };
            using ImageTypePointer = std::shared_ptr<ImageType>;

            ImageTypePointer Find(const size_t dimensions);

            using DataTypePointers = std::vector<ImageTypePointer>;
            DataTypePointers dataTypePointers;
        };

        // Constant storage.
        template<typename T>
        struct ConstantScalarStorage
        {
            using ScalarConstantMap = std::map<T, ConstantPointer<T>>;

            ScalarConstantMap constants;
        };

        using ConstantScalarStorages = std::tuple<ConstantScalarStorage<int32_t>, ConstantScalarStorage<float>>;

        template<size_t VDimensions, typename T>
        struct ConstantVectorStorage
        {
            using VectorType = Vector<VDimensions, T>;
            using Components = Vector<VDimensions, Spirv::Id>;

            struct ConstantData
            {
                ConstantPointer<VectorType> constant;
                Components components;
            };
            
            using VectorConstantMap = std::map<VectorType, ConstantData, SpirvGeneratorConstantVectorComparer<T, VDimensions>>;

            VectorConstantMap constants;
        };

        using ConstantVectorStorages = std::tuple<ConstantVectorStorage<2, float>, ConstantVectorStorage<3, float>, ConstantVectorStorage<4, float>>;


         // Generator input pin.
        struct GeneratorInputPin
        {
            explicit GeneratorInputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            GeneratorOutputPin* connectedGeneratorOutputPin;
            Spirv::Id connectedId;
            Spirv::Id loadId;
        }; 
        using GeneratorInputPinPointer = std::shared_ptr<GeneratorInputPin>;

        // Generator output pin.
        struct GeneratorOutputPin
        {
            explicit GeneratorOutputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            Spirv::Id id;
            Spirv::Id storeId;
        };
        using GeneratorOutputPinPointer = std::shared_ptr<GeneratorOutputPin>;

        // Generator node.
        struct GeneratorNode
        {
            using GeneratorInputPinPointers = std::vector<GeneratorInputPinPointer>;
            using GeneratorInputPinIterator = GeneratorInputPinPointers::iterator;
            using GeneratorOutputPinPointers = std::vector<GeneratorOutputPinPointer>;

            explicit GeneratorNode(const Visual::Node& node);

            GeneratorInputPin* GetNextInputPin();

            const Visual::Node& node;
            GeneratorInputPinPointers inputPins;
            GeneratorInputPinIterator inputPinIterator;
            GeneratorOutputPinPointers outputPins;

        private:

            static GeneratorInputPinPointers CreateInputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode);
            static GeneratorOutputPinPointers CreateOutputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode);

        };
        using GeneratorNodePointer = std::shared_ptr<GeneratorNode>;

        // Generator output interface pin.
        struct GeneratorOutputInterfacePin
        {
            GeneratorOutputInterfacePin(
                const Visual::Pin& pin,
                DataTypePointerDefinitionPointer dataTypePointerDefinition,
                const Spirv::Id id,
                const Spirv::Word location);

            const Visual::Pin& pin;
            const DataTypePointerDefinitionPointer dataTypePointerDefinition;
            const Spirv::Id id;
            const Spirv::Word location;
        };
        using GeneratorOutputInterfacePinPointer = std::shared_ptr<GeneratorOutputInterfacePin>;

        // Generator output interface.
        struct GeneratorOutputInterface
        {
            using Pins = std::vector<GeneratorOutputInterfacePinPointer>;
            using PinMap = std::map<const Visual::Pin*, GeneratorOutputInterfacePinPointer>;

            GeneratorOutputInterfacePinPointer Find(const Visual::Pin& pin);

            Pins pins;
            PinMap pinMap;
        };


        void InitGenerator(
            const Visual::Script& script,
            const bool includeDebugSymbols);

        void InitInterfaces();

        /** Script traversal functions. */
        /**@{*/
        [[nodiscard]] bool TraverseScriptNodes();
        [[nodiscard]] GeneratorNodePointer ProcessScriptInputPin(GeneratorInputPin* generatorInputPin);
        [[nodiscard]] bool ProcessScriptNode(GeneratorNodePointer& generatorNode);
        [[nodiscard]] bool ProcessScriptOperatorNode(GeneratorNodePointer& generatorNode);
        [[nodiscard]] bool ProcessScriptPushConstantNode(GeneratorNode& generatorNode);
        [[nodiscard]] bool ProcessScriptConstantNode(GeneratorNode& generatorNode);
        [[nodiscard]] bool ProcessScriptDescriptorBindingNode(GeneratorNode& generatorNode);
        [[nodiscard]] bool ProcessScriptInputNode(GeneratorNode& generatorNode);
        [[nodiscard]] bool ProcessScriptOutputNode(GeneratorNodePointer& generatorNode);
        /**@}*/

        /** Get or create data types and pointers. */
        /**@{*/
        DataTypeDefinitionPointer GetOrCreateDataType(const VariableDataType dataType);
        DataTypePointerDefinitionPointer GetOrCreateDataTypePointer(DataTypePointerStorage& storage, const VariableDataType dataType);
        /**@}*/

        /** Get or create constants. */
        /**@{*/
        template<typename T>
        ConstantPointer<T> GetOrCreateConstantScalar(const T value);

        template<size_t VDimensions, typename T>
        ConstantPointer<Vector<VDimensions, T>> GetOrCreateConstantVector(const Vector<VDimensions, T>& value);

        Spirv::Id GetOrCreateInputPinDefaultConstant(const Visual::Pin& pin);

        void CreateOutputInterfacePin(
            const Visual::Pin& pin,
            DataTypePointerDefinitionPointer dataTypePointerDefinition,
            const Spirv::Id id,
            const Spirv::Word location);
        /**@}*/

        GeneratorNodePointer CreateGeneratorNode(const Visual::Node& node);
        [[nodiscard]] Spirv::Id GetNextId();
        [[nodiscard]] std::string GetNextDebugName(const std::string& namePrefix);

        /** Module writing functions. */
        /**@{*/
        [[nodiscard]] Spirv::Words WriteModule();
        void WriteOperator(const GeneratorNodePointer& generatorNode, const Visual::OperatorBase& operatorBase);
        void WriteOperatorArithmetic(const GeneratorNodePointer& generatorNode, const Visual::ArithmeticOperatorBase& arithmeticOperatorBase);
        void WriteOutput(const GeneratorNodePointer& generatorNode, const Visual::OutputInterface& outputInterface);
        /**@}*/

        Logger* m_logger;
        const Visual::Script* m_script;
        bool m_includeDebugSymbols;
        Spirv::ModuleBuffer m_module;
        Spirv::Id m_currentId;
        std::vector<Spirv::Capability> m_capabilities;
        std::optional<Spirv::ExtensionImport> m_glslExtension;
        Spirv::EntryPoint m_mainEntryPoint;

        Spirv::Id m_voidTypeId;
        Spirv::Id m_mainFunctionTypeId;
        Spirv::Id m_mainFunctionLabelId;

        PushConstantStructPointer m_pushConstantStruct; // TODO: Replace?
        InputOutputStructPointer m_inputStruct; // TODO: Replace?

        DataTypeStorage m_dataTypes;
        ImageTypeStorage m_imageDataTypes;
        DataTypePointerStorage m_inputDataTypePointers;
        DataTypePointerStorage m_outputDataTypePointers;
        DataTypePointerStorage m_functionDataTypePointers;
        UniformBufferPointers m_uniformBuffers;
        //UniformBufferStorage m_uniformBufferStorage;

        ConstantScalarStorages m_constantScalarStorages;
        ConstantVectorStorages m_constantVectorStorages;

        std::vector<GeneratorNodePointer> m_outputNodes;
        
        std::map<const Visual::Node*, GeneratorNodePointer> m_nodes;
        std::map<const Visual::Pin*, GeneratorOutputPinPointer> m_outputPins;
        GeneratorOutputInterface m_outputInterface;
        
        std::map<Spirv::Id, std::string> m_debugNames;
        std::map<std::string, size_t> m_debugNameCounters;

        std::vector<GeneratorNodePointer> m_mainInstructions;

    };

}

#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.inl"

#endif
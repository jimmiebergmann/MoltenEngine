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
#include <variant>
#include <optional>

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
        struct DataType
        {
            explicit DataType(const VariableDataType type);

            Spirv::Id id;
            VariableDataType type;
            
        };
        using DataTypePointer = std::shared_ptr<DataType>;
        using DataTypePointers = std::vector<DataTypePointer>;

        // Data type pointer.
        struct DataTypePtr
        {
            DataTypePtr(
                DataTypePointer dataType,
                const Spirv::StorageClass storageClass);

            Spirv::Id id;
            DataTypePointer dataType;
            Spirv::StorageClass storageClass;
            
        };
        using DataTypePtrPointer = std::shared_ptr<DataTypePtr>;
        using DataTypePtrPointers = std::vector<DataTypePtrPointer>;

        // Constant.
        struct Constant
        {
            using Variant = std::variant<bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32>;

            template<typename T>
            Constant(DataTypePointer dataType, const T& value);

            Spirv::Id id;
            DataTypePointer dataType;
            Variant value;
        };
        using ConstantPointer = std::shared_ptr<Constant>;
        using ConstantPointers = std::vector<ConstantPointer>;
     
        /*
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
        using UniformBufferPointers = std::vector<UniformBufferPointer>;*/     

        // Image type storage.
        /*struct ImageTypeStorage
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
        };*/


        // Generator input pin.
        struct GeneratorInputPin
        {
            explicit GeneratorInputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            GeneratorOutputPin* connectedGeneratorOutputPin;
        }; 
        using GeneratorInputPinPointer = std::shared_ptr<GeneratorInputPin>;

        // Generator output pin.
        struct GeneratorOutputPin
        {
            explicit GeneratorOutputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            Spirv::Id id;
            Spirv::StorageClass storageClass;
            bool isVisited;
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

            GeneratorOutputPinPointer FindOutputPin(const Visual::Pin& pin);

            const Visual::Node& node;
            GeneratorInputPinPointers inputPins;
            GeneratorInputPinIterator inputPinIterator;
            GeneratorOutputPinPointers outputPins;

        private:

            static GeneratorInputPinPointers CreateInputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode);
            static GeneratorOutputPinPointers CreateOutputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode);

        };
        using GeneratorNodePointer = std::shared_ptr<GeneratorNode>;

        // Output structure
        struct OutputStructure
        {
            struct Member
            {
                Member(
                    GeneratorOutputPinPointer outputPin,
                    DataTypePointer dataType,
                    DataTypePtrPointer dataTypePointer = nullptr);

                GeneratorOutputPinPointer outputPin;
                DataTypePointer dataType;
                DataTypePtrPointer dataTypePointer;
            };

            void Clear();

            using Members = std::vector<Member>;

            Members members;
        };

        // Input structure
        struct InputStructure
        {
            struct Member
            {
                Member(
                    GeneratorInputPinPointer inputPin,
                    DataTypePointer dataType,
                    DataTypePtrPointer dataTypePointer = nullptr);

                Spirv::Id id;
                GeneratorInputPinPointer inputPin;
                DataTypePointer dataType;
                DataTypePtrPointer dataTypePointer;
            };

            Member* FindMember(GeneratorInputPinPointer& inputPin);

            void Clear();

            using Members = std::vector<Member>;

            Members members;
        };

        // Data type storage.
        class DataTypeStorage
        {

        public:

            DataTypePointer Get(const VariableDataType type);
            DataTypePointer GetOrCreate(const VariableDataType type);

            DataTypePointers GetAllDependencySorted();

            void Clear();

        private:

            struct DataTypePair
            {
                VariableDataType type;
                DataTypePointer dataType;
            };

            using DataTypePairs = std::vector<DataTypePair>;
            DataTypePairs m_dataTypes;
        };

        // Data type pointer storage
        class DataTypePointerStorage
        {

        public:

            DataTypePtrPointer Get(const Spirv::StorageClass storageClass, DataTypePointer& dataType);
            DataTypePtrPointer GetOrCreate(const Spirv::StorageClass storageClass, DataTypePointer& dataType);

            DataTypePtrPointers GetAll(const Spirv::StorageClass storageClass);

            void Clear();

        private:

            using DataTypePointersMap = std::map<Spirv::StorageClass, DataTypePtrPointers>;

            DataTypePointersMap m_dataTypePointers;

        };

        // Constant storage.
        class ConstantStorage
        {

        public:

            ConstantPointer Get(GeneratorInputPin& generatorInputPin);
            template<typename T>
            ConstantPointer Get(const T& value);

            ConstantPointer GetOrCreate(DataTypeStorage& dataTypeStorage, GeneratorInputPin& generatorInputPin);
            template<typename T>
            ConstantPointer GetOrCreate(DataTypeStorage& dataTypeStorage, const T& value);  

            ConstantPointers GetAllDependencySorted();

            void Clear();

        private:

            using ConstantMap = std::unordered_map<VariableDataType, ConstantPointers>;
            ConstantMap m_constants;

        };

        // Debug name storage
        class DebugNameStorage
        {

        public:

            struct NameEntry
            {
                NameEntry(
                    GeneratorOutputPinPointer& generatorOutputPin,
                    std::string name,
                    const size_t opNameOffset);

                GeneratorOutputPinPointer generatorOutputPin;
                const std::string name;
                const size_t opNameOffset;
            };

            using NameEntries = std::vector<NameEntry>;

            DebugNameStorage();

            void Add(GeneratorOutputPinPointer& generatorOutputPin, const std::string& name);
            void AddWithoutCounter(GeneratorOutputPinPointer& generatorOutputPin, const std::string& name);

            const NameEntries& GetEntries() const;

            void Clear();            

        private:

            NameEntries m_entries;
            std::map<std::string, size_t> m_debugNameCounters;
            size_t m_currentOpNameOffset;

        };

        void InitGenerator(
            const Visual::Script& script,
            const bool includeDebugSymbols);

        void InitInterfaces();

        /** Build tree functions. */
        /**@{*/
        [[nodiscard]] bool BuildTree();
        [[nodiscard]] GeneratorNodePointer BuildVisitInputPin(GeneratorNodePointer& generatorNode, GeneratorInputPin* generatorInputPin);
        void BuildVisitOutputPin(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin);
        void BuildVisitNode(GeneratorNodePointer& generatorNode);
        bool AddOutputPinDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin);
        [[nodiscard]] bool AddFunctionDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin);
        [[nodiscard]] bool AddOperatorDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin);
        /**@}*/

        [[nodiscard]] GeneratorNodePointer CreateGeneratorNode(const Visual::Node& node);
        [[nodiscard]] Spirv::Id GetNextId();
        [[nodiscard]] bool AccessNodeInputInMainFunction(GeneratorInputPin& generatorInputPin, std::vector<Spirv::Id>& inputIds);
        [[nodiscard]] Spirv::Id AccessOrTransformStorageClass(GeneratorOutputPin& generatorOutputPin, const Spirv::StorageClass storageClass);

        /** Module writing functions. */
        /**@{*/
        [[nodiscard]] bool WriteModule();
        void UpdateDataTypeIds();
        void UpdateInputPointerIds();
        void UpdateInputIds();
        void UpdateOutputPointerIds();
        void UpdateOutputIds();
        void WriteDebugNames();
        void WriteDecorations();
        void WriteInputDecorations();
        void WriteOutputDecorations();
        [[nodiscard]] bool WriteDataTypes();
        void WriteInputPointers();
        void WriteInputs();
        void WriteOutputPointers();
        void WriteOutputs();
        [[nodiscard]] bool WriteConstants();
        [[nodiscard]] bool WriteMainInstructions();
        [[nodiscard]] bool WriteMainInstruction(const GeneratorNodePointer& generatorNode);
        [[nodiscard]] bool WriteFunction(const GeneratorNodePointer& generatorNode, const Visual::FunctionBase& functionBase);
        [[nodiscard]] bool WriteOperator(const GeneratorNodePointer& generatorNode, const Visual::OperatorBase& operatorBase);
        [[nodiscard]] bool WriteOperatorArithmetic(const GeneratorNodePointer& generatorNode, const Visual::ArithmeticOperatorBase& arithmeticOperatorBase);
        [[nodiscard]] bool WriteOutput(const GeneratorNodePointer& generatorNode, const Visual::OutputInterface& outputInterface);
        void UpdateDebugNames();

        Logger* m_logger;
        const Visual::Script* m_script;
        bool m_includeDebugSymbols;
        Spirv::ModuleBuffer m_module;
        Spirv::Id m_currentId;
        std::vector<Spirv::Capability> m_capabilities;
        Spirv::ExtensionImport m_glslExtension;
        Spirv::EntryPoint m_mainEntryPoint;

        Spirv::Id m_voidTypeId;
        Spirv::Id m_mainFunctionTypeId;
        Spirv::Id m_mainFunctionLabelId;

        std::vector<GeneratorNodePointer> m_rootNodes;
        std::unordered_map<const Visual::Node*, GeneratorNodePointer> m_createdNodes;
        std::unordered_map<const Visual::Pin*, GeneratorOutputPinPointer> m_visitedOutputPins;

        DataTypeStorage m_dataTypeStorage;
        DataTypePointerStorage m_dataTypePointerStorage;
        ConstantStorage m_constantStorage; 
        OutputStructure m_inputStructure;
        InputStructure m_outputStructure;
        DebugNameStorage m_debugNameStorage;
        size_t m_opDebugNameStart;

        std::vector<GeneratorNodePointer> m_mainInstructions;

        //ImageTypeStorage m_imageDataTypes;
        //UniformBufferPointers m_uniformBuffers;
        //UniformBufferStorage m_uniformBufferStorage;
        //PushConstantStructPointer m_pushConstantStruct; // TODO: Replace?
    };

}

#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.inl"

#endif
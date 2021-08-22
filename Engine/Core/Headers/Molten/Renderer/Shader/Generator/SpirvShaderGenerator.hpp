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
        struct GeneratorInputPin;
        struct GeneratorOutputPin;
        struct DataType;
        struct DataTypePtr;
        struct Constant;
        struct OutputStructure;
        struct InputStructure;
        struct Structure;
        class DataTypeStorage;
        class DataTypePointerStorage;
        class ConstantStorage;
        class UniformStorage;
        class DebugNameStorage;
        
        using GeneratorNodePointer = std::shared_ptr<GeneratorNode>;
        using GeneratorInputPinPointer = std::shared_ptr<GeneratorInputPin>;
        using GeneratorOutputPinPointer = std::shared_ptr<GeneratorOutputPin>;
        using DataTypePointer = std::shared_ptr<DataType>;
        using DataTypePointers = std::vector<DataTypePointer>;
        using DataTypePtrPointer = std::shared_ptr<DataTypePtr>;
        using DataTypePtrPointers = std::vector<DataTypePtrPointer>;
        using ConstantPointer = std::shared_ptr<Constant>;
        using ConstantPointers = std::vector<ConstantPointer>;
        using StructurePointer = std::shared_ptr<Structure>;
        using IdPointer = std::shared_ptr<Spirv::Id>;

        // Generator input pin.
        struct GeneratorInputPin
        {
            explicit GeneratorInputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            GeneratorOutputPin* connectedGeneratorOutputPin;
        };

        // Generator output pin.
        struct GeneratorOutputPin
        {
            explicit GeneratorOutputPin(const Visual::Pin& pin);

            const Visual::Pin& pin;
            Spirv::Id id;
            Spirv::StorageClass storageClass;
            //bool isVisited;
        };

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

        // Data type.
        struct DataType
        {
            explicit DataType(const VariableDataType type);

            Spirv::Id id;
            VariableDataType type;
            
        };

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

        // Input structure
        struct InputStructure
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

            void AddMember(
                DataTypeStorage& dataTypeStorage,
                DataTypePointerStorage& dataTypePointerStorage,
                GeneratorOutputPinPointer& generatorOutputPin);

            void Clear();

            using Members = std::vector<Member>;

            Members members;
        };

        // Output structure
        struct OutputStructure
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

        // Push constant structure
        struct Structure
        {
            struct Member
            {
                Member(
                    Structure* structure,
                    GeneratorOutputPinPointer outputPin,
                    DataTypePointer dataType,
                    DataTypePtrPointer dataTypePointer);

                Structure* structure;
                Spirv::Id id;
                Spirv::Word index;
                Spirv::Word offset;
                ConstantPointer indexConstant;
                GeneratorOutputPinPointer outputPin;
                DataTypePointer dataType;
                DataTypePtrPointer dataTypePointer;
            };

            explicit Structure(const Spirv::StorageClass storageClass);

            void AddMember(
                DataTypeStorage& dataTypeStorage,
                DataTypePointerStorage& dataTypePointerStorage,
                GeneratorOutputPinPointer& generatorOutputPin);

            Member* FindMember(const Visual::Pin* pin);

            void Clear();

            using Members = std::vector<Member>;

            Spirv::Id id;
            Spirv::Id typeId;
            Spirv::Id typePointerId;
            Members members;
            bool isEmpty;
            const Spirv::StorageClass storageClass;
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
            ConstantPointer Get(const Visual::ConstantBase& constantBase);
            template<typename T>
            ConstantPointer Get(const T& value);

            ConstantPointer GetOrCreate(DataTypeStorage& dataTypeStorage, GeneratorInputPin& generatorInputPin);
            ConstantPointer GetOrCreate(DataTypeStorage& dataTypeStorage, const Visual::ConstantBase& constantBase);
            template<typename T>
            ConstantPointer GetOrCreate(DataTypeStorage& dataTypeStorage, const T& value);  

            ConstantPointers GetAllDependencySorted();

            void Clear();

        private:

            using ConstantMap = std::unordered_map<VariableDataType, ConstantPointers>;
            ConstantMap m_constants;

        };

        // Uniform storage.
        class SamplerStorage
        {

        public:

            struct Sampler
            {
                Sampler(
                    const Spirv::Id setId,
                    const Spirv::Id bindingId,
                    GeneratorOutputPinPointer outputPin,
                    DataTypePointer dataType,
                    DataTypePtrPointer dataTypePointer);

                Spirv::Id setId;
                Spirv::Id bindingId;
                GeneratorOutputPinPointer outputPin;
                DataTypePointer dataType;
                DataTypePtrPointer dataTypePointer;
            };

            using Samplers = std::vector<Sampler>;

            void Add(
                DataTypeStorage& dataTypeStorage,
                DataTypePointerStorage& dataTypePointerStorage,
                GeneratorOutputPinPointer& generatorOutputPin,
                const Visual::DescriptorBindingBase& descriptorBindingBase);

            Samplers& GetSamplers();

            void Clear();

        private:
            
            Samplers m_samplers;

        };

        // Uniform buffer storage
        class UniformBufferStorage
        {

        public:

            using SetAndBindingPair = std::pair<size_t, size_t>;
            using UniforBufferMap = std::map<SetAndBindingPair, StructurePointer>;

            void Add(
                DataTypeStorage& dataTypeStorage,
                DataTypePointerStorage& dataTypePointerStorage,
                GeneratorOutputPinPointer& generatorOutputPin,
                const Visual::DescriptorBindingBase& descriptorBindingBase);

            Structure::Member* FindMember(const Visual::Pin* pin);

            UniforBufferMap& GetBuffers();

            void Clear();

        private:

            UniforBufferMap m_uniformBufferMap;
            
        };

        // Debug name storage
        class DebugNameStorage
        {

        public:

            DebugNameStorage();

            void SetDebugNamesOffset(const size_t offset);
            size_t GetDebugNamesOffset() const;

            void Add(const Spirv::Id targetId, const std::string& name);
            void AddWithoutCounter(const Spirv::Id targetId, const std::string& name);

            Spirv::ModuleBuffer& GetBuffer();

            void Clear();            

        private:

            std::map<std::string, size_t> m_debugNameCounters;
            Spirv::ModuleBuffer m_buffer;
            size_t m_debugNamesOffset;

        };

        void InitGenerator(
            const Visual::Script& script,
            const bool includeDebugSymbols);

        /** Build tree functions. */
        /**@{*/
        [[nodiscard]] bool BuildTree();
        [[nodiscard]] GeneratorNodePointer BuildVisitInputPin(GeneratorNodePointer& generatorNode, GeneratorInputPin* generatorInputPin);
        void BuildVisitOutputPin(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin);
        void BuildVisitNode(GeneratorNodePointer& generatorNode);
        /**@}*/

        [[nodiscard]] GeneratorNodePointer CreateGeneratorNode(const Visual::Node& node);
        [[nodiscard]] Spirv::Id GetNextId(const Spirv::Id incrementor = 1);
        [[nodiscard]] bool AccessNodeInputInMain(GeneratorInputPin& generatorInputPin, std::vector<Spirv::Id>& inputIds);
        [[nodiscard]] Spirv::Id AccessOrTransformStorageClassInMain(GeneratorOutputPin& generatorOutputPin);

        /** Module writing functions. */
        /**@{*/
        [[nodiscard]] bool WriteModule();

        void UpdatePushConstantMembers();
        void UpdateUniformBuffersMembers();

        void UpdateDataTypeIds();
        void UpdateInputPointerIds();
        void UpdateInputIds();
        void UpdateOutputPointerIds();
        void UpdateOutputIds();
        void UpdatePushConstantPointerIds();
        void UpdatePushConstantStruct();
        void UpdateUniformConstantPointers();
        void UpdateSamplerIds();
        void UpdateUniformPointerIds();
        void UpdateUniformBufferStructs();

        void AddGlobalDebugNames();

        void WriteDecorations();
        void WriteInputDecorations();
        void WriteOutputDecorations();
        void WritePushConstantDecorations();
        void WriteSamplerDecorations();
        void WriteUniformBufferDecorations();

        [[nodiscard]] bool WriteDataTypes();
        void WriteInputPointers();
        void WriteInputs();
        void WriteOutputPointers();
        void WriteOutputs();
        void WritePushConstantStruct();
        void WritePushConstantPointers();
        void WriteUniformConstantPointers();
        void WriteSamplers();
        void WriteUniformBufferStructs();
        void WriteUniformPointerIds();
        [[nodiscard]] bool WriteConstants();

        [[nodiscard]] bool WriteMainInstructions();
        [[nodiscard]] bool WriteMainInstruction(const GeneratorNodePointer& generatorNode);
        [[nodiscard]] bool WriteFunction(const GeneratorNodePointer& generatorNode, const Visual::FunctionBase& functionBase);
        [[nodiscard]] bool WriteOperator(const GeneratorNodePointer& generatorNode, const Visual::OperatorBase& operatorBase);
        [[nodiscard]] bool WriteOperatorArithmetic(const GeneratorNodePointer& generatorNode, const Visual::ArithmeticOperatorBase& arithmeticOperatorBase);
        [[nodiscard]] bool WriteOutput(const GeneratorNodePointer& generatorNode, const Visual::OutputInterface& outputInterface);
        [[nodiscard]] bool WriteConstant(const GeneratorNodePointer& generatorNode, const Visual::ConstantBase& constantBase);
        void InsertDebugNames();

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
        InputStructure m_inputStructure;
        OutputStructure m_outputStructure;
        Structure m_pushConstantStructure;
        SamplerStorage m_samplerStorage;
        UniformBufferStorage m_uniformBufferStorage;
        DebugNameStorage m_debugNameStorage;

        std::vector<GeneratorNodePointer> m_mainInstructions;

        //ImageTypeStorage m_imageDataTypes;
        //UniformBufferPointers m_uniformBuffers;
        //UniformBufferStorage m_uniformBufferStorage;
        //PushConstantStructPointer m_pushConstantStruct; // TODO: Replace?
    };

}

#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.inl"

#endif
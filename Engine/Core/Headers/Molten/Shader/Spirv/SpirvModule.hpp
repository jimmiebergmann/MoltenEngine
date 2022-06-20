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

#ifndef MOLTEN_CORE_SHADER_SPIRV_SPIRVMODULE_HPP
#define MOLTEN_CORE_SHADER_SPIRV_SPIRVMODULE_HPP

#include "Molten/Math/Vector.hpp"
#include <vector>
#include <string>

namespace Molten::Shader::Spirv
{
    using Word = uint32_t;
    using Words = std::vector<Word>;
    using HalfWord = uint16_t;
    using Id = Word;

    enum class OpCode : HalfWord
    {
        Nop = 0,
        Source = 3,
        Name = 5,
        MemberName = 6,
        ExtInstImport = 11,
        ExtInst = 12,
        MemoryModel = 14,
        EntryPoint = 15,
        ExecutionMode = 16,
        Capability = 17,
        TypeVoid = 19,
        TypeBool = 20,
        TypeInt = 21,
        TypeFloat = 22,
        TypeVector = 23,
        TypeMatrix = 24,
        TypeImage = 25,
        TypeSampledImage = 27,
        TypeStruct = 30,
        TypePointer = 32,
        TypeFunction = 33,
        Constant = 43,
        ConstantComposite = 44,
        Function = 54,
        FunctionEnd = 56,
        Variable = 59,
        Load = 61,
        Store = 62,
        AccessChain = 65,
        Decorate = 71,
        MemberDecorate = 72,
        CompositeConstruct = 80,
        CompositeExtract = 81,
        ImageSampleImplicitLod = 87,
        FAdd = 129,
        FSub = 131,
        FMul = 133,
        FDiv = 136,
        VectorTimesMatrix = 144,
        MatrixTimesVector = 145,
        Label = 248,
        Return = 253
    };

    enum class GlslInstruction : Word
    {
        Sin = 13,
        Cos = 14,
        Tan = 15,
        FMin = 37,
        FMax = 40,
        Cross = 68
    };

    enum class AddressingModel : Word
    {
        Logical = 0,
        Physical32 = 1,
        Physical64 = 2
    };

    enum class MemoryModel : Word
    {
        Simple = 0,
        Glsl450 = 1,
        OpenCl = 2
    };

    enum class Capability : Word
    {
        Matrix = 0,
        Shader = 1
    };

    enum class ExecutionModel : Word
    {
        Vertex = 0,
        TessellationControl = 1,
        TessellationEvaluation = 2,
        Geometry = 3,
        Fragment = 4,
        GlCompute = 5,
        Kernel = 6
    };

    enum class SourceLanguage : Word
    {
        Unknown = 0,
        Elsl = 1,
        Glsl = 2,
        OpenClC = 3,
        OpenClCpp = 4,
        Hlsl = 5
    };

    enum class ExecutionMode : Word
    {
        OriginUpperLeft = 7
    };

    enum class Decoration : Word
    {
        Block = 2,
        ColMajor = 5,
        MatrixStride = 7,
        BuiltIn = 11,
        Location = 30,
        Binding = 33,
        DescriptorSet = 34,
        Offset = 35
    };

    enum class Signedness : Word
    {
        Unsigned = 0,
        Signed = 1
    };

    enum class FunctionControl : Word
    {
        None = 0,
        Inline = 1,
        DontInline = 2,
        Pure = 4,
        Const = 8
    };

    enum class StorageClass : Word
    {
        UniformConstant = 0,
        Input = 1,
        Uniform = 2,
        Output = 3,
        Function = 7,
        PushConstant = 9
    };

    enum class Dimensionality : Word
    {
        Image1D = 0,
        Image2D = 1,
        Image3D = 2,
        Cube = 3,
        Rect = 4,
        Buffer = 5,
        SubpassData = 6
    };

    enum class BuiltIn : Word
    {
        Position = 0,
        PointSize = 1,
        ClipDistance = 3,
        CullDistance = 4
    };

    struct ExtensionImport
    {
        Id resultId;
        std::string name;
    };

    struct EntryPoint
    {
        ExecutionModel executionModel;
        Id id;
        std::string name;
        std::vector<Id> interfaceIds;
    };

    [[nodiscard]] Word MOLTEN_API CreateOpCode(const OpCode opCode, const HalfWord wordCount);
    [[nodiscard]] HalfWord GetLiteralWordCount(const size_t stringLength);

    class ModuleBuffer
    {

    public:

        void AddHeader(const Word version, const Word generatorMagicNumber, const Word idBound = 0, const Word reserved = 0);
        void AddOpExtInstImport(const ExtensionImport& extensionImport);
        void AddOpExtInst(const Id resultTypeId, const Id resultId, const Id setId, const Word instruction, const std::vector<Id>& inputIds);
        void AddOpMemoryModel(const AddressingModel addressingModel, const MemoryModel memoryModel);
        void AddOpEntryPoint(const EntryPoint& entryPoint);
        void AddOpCapability(const Capability capability);
        void AddOpExecutionMode(const Id entryPointId, const ExecutionMode executionMode);

        void AddOpName(const Id targetId, const std::string& name);
        void AddOpMemberName(const Id targetId, const Word memberIndex, const std::string& name);

        void AddOpDecorateBlock(const Id targetId);
        void AddOpDecorateDescriptorSet(const Id targetId, const Id descriptorSetId);
        void AddOpDecorateBinding(const Id targetId, const Id bindingId);
        void AddOpDecorateLocation(const Id targetId, const Id locationId);
        void AddOpMemberDecorateOffset(const Id structureTypeId, const Word memberIndex, const Word byteOffset);
        void AddOpMemberDecorateBuiltIn(const Id structureTypeId, const Word memberIndex, const BuiltIn builtIn);
        void AddOpMemberDecorateColMajor(const Id structureTypeId, const Word memberIndex);
        void AddOpMemberDecorateMatrixStride(const Id structureTypeId, const Word memberIndex, const uint32_t stride);

        void AddOpTypeVoid(const Id resultId);
        void AddOpTypeBool(const Id resultId);
        void AddOpTypeInt32(const Id resultId, Signedness signedness);
        void AddOpTypeFloat32(const Id resultId);
        void AddOpTypeVector(const Id resultId, const Id componentTypeId, Word componentCount);
        void AddOpTypeMatrix(const Id resultId, const Id columnTypeId, Word columnCount);
        void AddOpTypeImage(const Id resultId, const Id sampledTypeId, const Dimensionality dimmensions);
        void AddOpTypeSampledImage(const Id resultId, const Id imageTypeId);
        void AddOpTypeStruct(const Id resultId, const std::vector<Id>& memberTypeIds);
        void AddOpTypePointer(const Id resultId, const StorageClass storageClass, const Id typeId);
        void AddOpTypeFunction(const Id resultId, const Id returnTypeId);

        void AddOpConstantBool(const Id resultId, const Id resultTypeId, const bool value);
        void AddOpConstantInt32(const Id resultId, const Id resultTypeId, const int32_t value);
        void AddOpConstantFloat32(const Id resultId, const Id resultTypeId, const float value);
        void AddOpConstantVector2(const Id resultId, const Id resultTypeId, const Vector2<Id>& valueIds);
        void AddOpConstantVector3(const Id resultId, const Id resultTypeId, const Vector3<Id>& valueIds);
        void AddOpConstantVector4(const Id resultId, const Id resultTypeId, const Vector4<Id>& valueIds);

        void AddOpVariable(const Id resultId, const Id resultTypeId, const StorageClass storageClass);
        void AddOpLoad(const Id resultTypeId, const Id resultId, const Id pointerId);
        void AddOpStore(const Id pointerId, const Id objectId);
        void AddOpAccessChain(const Id resultTypeId, const Id resultId, const Id baseId, const Id constantIndexId);

        void AddOpCompositeConstruct(const Id resultTypeId, const Id resultId, const std::vector<Id>& inputIds);
        void AddOpCompositeExtract(const Id resultTypeId, const Id resultId, const Id compositeId, uint32_t index);
        void AddOpImageSampleImplicitLod(const Id resultTypeId, const Id resultId, const Id sampledImageId, const Id coordinateId);
        void AddOpFAdd(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id);
        void AddOpFSub(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id);
        void AddOpFMul(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id);
        void AddOpFDiv(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id);
        void AddOpVectorTimesMatrix(const Id resultTypeId, const Id resultId, const Id vectorId, const Id matrixId);
        void AddOpMatrixTimesVector(const Id resultTypeId, const Id resultId, const Id matrixId, const Id vectorId);

        void AddOpFunction(const Id resultId, const Id resultTypeId, const FunctionControl functionControl, const Id functionTypeId);
        void AddOpLabel(const Id resultId);

        void AddOpReturn();
        void AddOpFunctionEnd();

        bool UpdateIdBound(const Word idBound);

        Words words;

    private:

        void AddLiteral(const std::string& string);

    };

}

#endif
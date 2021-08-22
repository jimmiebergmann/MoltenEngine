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

#include "Molten/Renderer/Shader/Spirv/SpirvModule.hpp"
#include <array>

namespace Molten::Shader::Spirv
{

    // Global implementations.
    [[nodiscard]] Word CreateOpCode(const OpCode opCode, const HalfWord wordCount)
    {
        return (static_cast<Word>(opCode)) | (static_cast<Word>(wordCount) << 16);
    }

    [[nodiscard]] HalfWord GetLiteralWordCount(const size_t stringLength)
    {
        return static_cast<HalfWord>(stringLength / 4) + 1;
    }


    // Spirv module buffer implementations.
    void ModuleBuffer::AddHeader(const Word version, const Word generatorMagicNumber, const Word idBound, const Word reserved)
    {
        const std::array<Word, 5> result =
        {
            0x07230203, // SPIR-V magic number.
            version,
            generatorMagicNumber,
            idBound,
            reserved
        };

        words.insert(words.begin(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpExtInstImport(const ExtensionImport& extensionImport)
    {
        const auto literalWordCount = GetLiteralWordCount(extensionImport.name.size());

        words.push_back(CreateOpCode(OpCode::ExtInstImport, literalWordCount + 2));
        words.push_back(extensionImport.resultId);
        AddLiteral(extensionImport.name);
    }

    void ModuleBuffer::AddOpExtInst(const Id resultTypeId, const Id resultId, const Id setId, const Word instruction, const std::vector<Id>& inputIds)
    {
        const size_t resultSize = 5 + inputIds.size();

        std::vector<Word> result(resultSize);

        result[0] = CreateOpCode(OpCode::ExtInst, static_cast<HalfWord>(resultSize));
        result[1] = static_cast<Word>(resultTypeId);
        result[2] = static_cast<Word>(resultId);
        result[3] = static_cast<Word>(setId);
        result[4] = static_cast<Word>(instruction);

        std::copy(inputIds.begin(), inputIds.end(), result.begin() + 5);

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpMemoryModel(const AddressingModel addressingModel, const MemoryModel memoryModel)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::MemoryModel , 3),
            static_cast<Word>(addressingModel),
            static_cast<Word>(memoryModel)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpEntryPoint(const EntryPoint& entryPoint)
    {
        const auto literalWordCount = GetLiteralWordCount(entryPoint.name.size());
        const HalfWord wordCount = literalWordCount + static_cast<HalfWord>(entryPoint.interface.size()) + 3;

        words.push_back(CreateOpCode(OpCode::EntryPoint, wordCount));
        words.push_back(static_cast<Word>(entryPoint.executionModel));
        words.push_back(static_cast<Word>(entryPoint.id));
        AddLiteral(entryPoint.name);
        words.insert(words.end(), entryPoint.interface.begin(), entryPoint.interface.end());
    }

    void ModuleBuffer::AddOpCapability(const Capability capability)
    {
        const std::array<Word, 2> result =
        {
            CreateOpCode(OpCode::Capability, 2),
            static_cast<Word>(capability)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpExecutionMode(const Id entryPointId, const ExecutionMode executionMode)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::ExecutionMode, 3),
            static_cast<Word>(entryPointId),
            static_cast<Word>(executionMode)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpName(const Id targetId, const std::string& name)
    {
        const auto literalWordCount = GetLiteralWordCount(name.size());

        words.push_back(CreateOpCode(OpCode::Name, literalWordCount + 2));
        words.push_back(targetId);
        AddLiteral(name);
    }

    void ModuleBuffer::AddOpMemberName(const Id targetId, const Word memberIndex, const std::string& name)
    {
        const auto literalWordCount = GetLiteralWordCount(name.size());

        words.push_back(CreateOpCode(OpCode::Name, literalWordCount + 3));
        words.push_back(targetId);
        words.push_back(memberIndex);
        AddLiteral(name);
    }

    void ModuleBuffer::AddOpDecorateBlock(const Id targetId)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::Decorate, 3),
            static_cast<Word>(targetId),
            static_cast<Word>(Decoration::Block)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpDecorateDescriptorSet(const Id targetId, const Id descriptorSetId)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Decorate, 4),
            static_cast<Word>(targetId),
            static_cast<Word>(Decoration::DescriptorSet),
            static_cast<Word>(descriptorSetId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpDecorateBinding(const Id targetId, const Id bindingId)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Decorate, 4),
            static_cast<Word>(targetId),
            static_cast<Word>(Decoration::Binding),
            static_cast<Word>(bindingId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpDecorateLocation(const Id targetId, const Id locationId)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Decorate, 4),
            static_cast<Word>(targetId),
            static_cast<Word>(Decoration::Location),
            static_cast<Word>(locationId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpMemberDecorateOffset(const Id structureTypeId, const Word memberIndex, const Word byteOffset)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::MemberDecorate, 5),
            static_cast<Word>(structureTypeId),
            memberIndex,
            static_cast<Word>(Decoration::Offset),
            byteOffset,
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeVoid(const Id resultId)
    {
        const std::array<Word, 2> result =
        {
            CreateOpCode(OpCode::TypeVoid, 2),
            static_cast<Word>(resultId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeBool(const Id resultId)
    {
        const std::array<Word, 2> result =
        {
            CreateOpCode(OpCode::TypeBool, 2),
            static_cast<Word>(resultId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeInt32(const Id resultId, Signedness signedness)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::TypeInt, 4),
            static_cast<Word>(resultId),
            32,
            static_cast<Word>(signedness)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeFloat32(const Id resultId)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::TypeFloat, 3),
            static_cast<Word>(resultId),
            32
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeVector(const Id resultId, const Id componentTypeId, Word componentCount)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::TypeVector, 4),
            static_cast<Word>(resultId),
            static_cast<Word>(componentTypeId),
            componentCount,
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeImage(const Id resultId, const Id sampledTypeId, const Dimensionality dimmensions)
    {
        const std::array<Word, 9> result =
        {
            CreateOpCode(OpCode::TypeImage, 9),
            static_cast<Word>(resultId),
            static_cast<Word>(sampledTypeId),
            static_cast<Word>(dimmensions),
            Word{ 0 },
            Word{ 0 },
            Word{ 0 },
            Word{ 1 },
            Word{ 0 }
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeSampledImage(const Id resultId, const Id imageTypeId)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::TypeSampledImage, 3),
            static_cast<Word>(resultId),
            static_cast<Word>(imageTypeId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeStruct(const Id resultId, const std::vector<Id>& memberTypeIds)
    {
        const std::array<Word, 2> result =
        {
            CreateOpCode(OpCode::TypeStruct, 2 + static_cast<HalfWord>(memberTypeIds.size())),
            static_cast<Word>(resultId)
        };

        words.insert(words.end(), result.begin(), result.end());
        words.insert(words.end(), memberTypeIds.begin(), memberTypeIds.end());
    }

    void ModuleBuffer::AddOpTypePointer(const Id resultId, const StorageClass storageClass, const Id typeId)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::TypePointer, 4),
            static_cast<Word>(resultId),
            static_cast<Word>(storageClass),
            static_cast<Word>(typeId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpTypeFunction(const Id resultId, const Id returnTypeId)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::TypeFunction, 3),
            static_cast<Word>(resultId),
            static_cast<Word>(returnTypeId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantBool(const Id resultId, const Id resultTypeId, const bool value)
    {
        const Word resultValue = value ? 1 : 0;

        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Constant, 4),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            resultValue
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantInt32(const Id resultId, const Id resultTypeId, const int32_t value)
    {
        Word resultValue = 0;
        std::memcpy(&resultValue, &value, std::min(sizeof(int32_t), sizeof(resultValue)));

        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Constant, 4),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            resultValue
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantFloat32(const Id resultId, const Id resultTypeId, const float value)
    {
        Word resultValue = 0;
        std::memcpy(&resultValue, &value, std::min(sizeof(float), sizeof(resultValue)));

        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Constant, 4),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            resultValue
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantVector2(const Id resultId, const Id resultTypeId, const Vector2<Id>& valueIds)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::ConstantComposite, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(valueIds.x),
            static_cast<Word>(valueIds.y)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantVector3(const Id resultId, const Id resultTypeId, const Vector3<Id>& valueIds)
    {
        const std::array<Word, 6> result =
        {
            CreateOpCode(OpCode::ConstantComposite, 6),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(valueIds.x),
            static_cast<Word>(valueIds.y),
            static_cast<Word>(valueIds.z)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpConstantVector4(const Id resultId, const Id resultTypeId, const Vector4<Id>& valueIds)
    {
        const std::array<Word, 7> result =
        {
            CreateOpCode(OpCode::ConstantComposite, 7),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(valueIds.x),
            static_cast<Word>(valueIds.y),
            static_cast<Word>(valueIds.z),
            static_cast<Word>(valueIds.w)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpVariable(const Id resultId, const Id resultTypeId, const StorageClass storageClass)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Variable, 4),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(storageClass)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpLoad(const Id resultTypeId, const Id resultId, const Id pointerId)
    {
        const std::array<Word, 4> result =
        {
            CreateOpCode(OpCode::Load, 4),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(pointerId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpStore(const Id pointerId, const Id objectId)
    {
        const std::array<Word, 3> result =
        {
            CreateOpCode(OpCode::Store, 3),
            static_cast<Word>(pointerId),
            static_cast<Word>(objectId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpAccessChain(const Id resultTypeId, const Id resultId, const Id baseId, const Id constantIndexId)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::AccessChain, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(baseId),
            static_cast<Word>(constantIndexId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpImageSampleImplicitLod(const Id resultTypeId, const Id resultId, const Id sampledImageId, const Id coordinateId)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::ImageSampleImplicitLod, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(sampledImageId),
            static_cast<Word>(coordinateId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpFAdd(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::FAdd, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(operand1Id),
            static_cast<Word>(operand2Id)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpFSub(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::FSub, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(operand1Id),
            static_cast<Word>(operand2Id)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpFMul(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::FMul, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(operand1Id),
            static_cast<Word>(operand2Id)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpFDiv(const Id resultTypeId, const Id resultId, const Id operand1Id, const Id operand2Id)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::FDiv, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(operand1Id),
            static_cast<Word>(operand2Id)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpFunction(const Id resultId, const Id resultTypeId, const FunctionControl functionControl, const Id functionTypeId)
    {
        const std::array<Word, 5> result =
        {
            CreateOpCode(OpCode::Function, 5),
            static_cast<Word>(resultTypeId),
            static_cast<Word>(resultId),
            static_cast<Word>(functionControl),
            static_cast<Word>(functionTypeId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpLabel(const Id resultId)
    {
        const std::array<Word, 2> result =
        {
            CreateOpCode(OpCode::Label, 2),
            static_cast<Word>(resultId)
        };

        words.insert(words.end(), result.begin(), result.end());
    }

    void ModuleBuffer::AddOpReturn()
    {
        words.push_back(CreateOpCode(OpCode::Return, 1));
    }

    void ModuleBuffer::AddOpFunctionEnd()
    {
        words.push_back(CreateOpCode(OpCode::FunctionEnd, 1));
    }

    bool ModuleBuffer::UpdateIdBound(const Word idBound)
    {
        if(words.size() < 4)
        {
            return false;
        }

        words[3] = idBound;

        return true;
    }

    void ModuleBuffer::AddLiteral(const std::string& string)
    {
        Word word = 0;
        size_t shift = 0;

        for(const auto c : string)
        {
            word |= static_cast<Word>(c) << shift;

            shift += 8;
            if (shift == 32)
            {
                words.push_back(word);
                word = 0;
                shift = 0;
            }
        }

        words.push_back(word);
    }

}
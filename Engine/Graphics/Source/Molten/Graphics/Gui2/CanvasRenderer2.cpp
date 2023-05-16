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

#include "Molten/Graphics/Gui2/CanvasRenderer2.hpp"
#include "Molten/Graphics/Renderer.hpp"
#include "Molten/Graphics/CommandBuffer.hpp"
#include "Molten/Graphics/Pipeline.hpp"
#include "Molten/Logger.hpp"
#include "Molten/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    std::unique_ptr<CanvasRenderer> CanvasRenderer::Create(const CanvasRendererDescriptor& descriptor)
    {
        auto& backendRenderer = descriptor.backendRenderer;
        auto* logger = descriptor.logger;

        SamplerDescriptor2D samplerDescriptor;
        samplerDescriptor.magFilter = SamplerFilter::Nearest;
        samplerDescriptor.minFilter = SamplerFilter::Nearest;
        samplerDescriptor.wrapModes = { SamplerWrapMode::Repeat, SamplerWrapMode::Repeat };
        auto sampler2D = backendRenderer.CreateSampler(samplerDescriptor);
        if (!sampler2D)
        {
            Logger::WriteError(logger, "Failed to create texture sampler for canvas renderer.");
            return nullptr;
        }

        auto coloredQuad = CreateColoredRect(descriptor);
        if (!coloredQuad.has_value())
        {
            return nullptr;
        }

        return std::unique_ptr<CanvasRenderer>(new CanvasRenderer(
            backendRenderer,
            std::move(sampler2D),
            std::move(coloredQuad.value())
        ));
    }

    void CanvasRenderer::Resize(const Vector2f32& size)
    {
        m_projection = Matrix4x4f32::Orthographic(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);
    }

    void CanvasRenderer::SetCommandBuffer(CommandBuffer& commandBuffer)
    {
        m_commandBuffer = &commandBuffer;
    }

    void CanvasRenderer::DrawQuad(const AABB2f32& bounds, const Vector4f32& color)
    {
        m_commandBuffer->BindPipeline(*m_coloredQuad.pipeline);

        m_commandBuffer->PushConstant(m_coloredQuad.projectionLocation, m_projection);
        m_commandBuffer->PushConstant(m_coloredQuad.positionLocation, bounds.position);
        m_commandBuffer->PushConstant(m_coloredQuad.sizeLocation, bounds.size);
        m_commandBuffer->PushConstant(m_coloredQuad.colorLocation, color);

        m_commandBuffer->DrawVertexBuffer(*m_coloredQuad.indexBuffer, *m_coloredQuad.vertexBuffer);
    }

    CanvasRenderer::CanvasRenderer(
        Renderer& backendRenderer,
        SharedRenderResource<Sampler2D> sampler2D,
        ColoredQuad coloredQuad
    ) :
        m_backendRenderer(backendRenderer),
        m_commandBuffer(nullptr),
        m_projection(Matrix4x4f32::Identity()),
        m_sampler2D(std::move(sampler2D)),
        m_coloredQuad(std::move(coloredQuad))
    {}

    std::optional<CanvasRenderer::ColoredQuad> CanvasRenderer::CreateColoredRect(const CanvasRendererDescriptor& descriptor)
    {
        auto& backendRenderer = descriptor.backendRenderer;
        auto* logger = descriptor.logger;

        const auto vertexData = std::array<Vector2f32, 4>{
            Vector2f32{ 0.0f, 0.0f },
            Vector2f32{ 1.0f, 0.0f },
            Vector2f32{ 1.0f, 1.0f },
            Vector2f32{ 0.0f, 1.0f }
        };

        const auto indices = std::array<uint16_t, 6>{
            0, 1, 2,
            0, 2, 3
        };

        const auto vertexPositionBufferDesc = VertexBufferDescriptor{
            .vertexCount = static_cast<uint32_t>(vertexData.size()),
            .vertexSize = sizeof(Vector2f32),
            .data = static_cast<const void*>(vertexData.data())
        };

        auto vertexBuffer = backendRenderer.CreateVertexBuffer(vertexPositionBufferDesc);
        if (!vertexBuffer)
        {
            Logger::WriteError(logger, "Failed to create position vertex buffer for canvas renderer(colored quad).");
            return std::nullopt;
        }

        const auto indexBufferDesc = IndexBufferDescriptor{
            .indexCount = static_cast<uint32_t>(indices.size()),
            .data = static_cast<const void*>(indices.data()),
            .dataType = IndexBuffer::DataType::Uint16
        };

        auto indexBuffer = backendRenderer.CreateIndexBuffer(indexBufferDesc);
        if (!indexBuffer)
        {
            Logger::WriteError(logger, "Failed to create position index buffer for canvas renderer(colored quad).");
            return std::nullopt;
        }

        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        {
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexPosition = inputs.AddMember<Vector2f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(1);
            auto& position = pushConstants.AddMember<Vector2f32>(2);
            auto& size = pushConstants.AddMember<Vector2f32>(3);

            auto* outPosition = script.GetVertexOutput();

            auto& vertexScaled = script.CreateOperator<Shader::Visual::Operators::MultVec2f32>();
            vertexScaled.GetLeftInput().Connect(vertexPosition);
            vertexScaled.GetRightInput().Connect(size);

            auto& vertexScaledMoved = script.CreateOperator<Shader::Visual::Operators::AddVec2f32>();
            vertexScaledMoved.GetLeftInput().Connect(vertexScaled.GetOutput());
            vertexScaledMoved.GetRightInput().Connect(position);

            auto& vertexPositionVec4 = script.CreateComposite<Shader::Visual::Composites::Vec4f32FromVec2f32Float32>();
            vertexPositionVec4.GetInput<0>().Connect(vertexScaledMoved.GetOutput());
            vertexPositionVec4.GetInput<1>().SetDefaultValue(0.0f);
            vertexPositionVec4.GetInput<2>().SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4f32Vec4f32>();
            projectedVertexPosition.GetLeftInput().Connect(projection);
            projectedVertexPosition.GetRightInput().Connect(vertexPositionVec4.GetOutput());

            outPosition->GetInputPin()->ConnectBase(*projectedVertexPosition.GetOutputPin());
        }
        {
            auto& script = fragmentScript;

            auto& pushConstants = script.GetPushConstants();
            auto& vertexColor = pushConstants.AddMember<Vector4f32>(4);

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(vertexColor);
        }

        const auto shaderProgramDesc = VisualShaderProgramDescriptor{
            .vertexScript = &vertexScript,
            .fragmentScript = &fragmentScript
        };

        auto shaderProgram = backendRenderer.CreateShaderProgram(shaderProgramDesc);
        if (!shaderProgram)
        {
            Logger::WriteError(logger, "Failed to create shader program for canvas renderer(colored quad).");
            return std::nullopt;
        }

        const auto pipelineDesc = PipelineDescriptor{
            .topology = Pipeline::Topology::TriangleList,
            .polygonMode = Pipeline::PolygonMode::Fill,
            .frontFace = Pipeline::FrontFace::Clockwise,
            .cullMode = Pipeline::CullMode::None,
            .renderPass = backendRenderer.GetSwapChainRenderPass(),
            .shaderProgram = shaderProgram
        };

        auto pipeline = backendRenderer.CreatePipeline(pipelineDesc);
        if (!pipeline)
        {
            Logger::WriteError(logger, "Failed to create pipeline for canvas renderer(colored quad).");
            return std::nullopt;
        }

        const auto projectionLocation = backendRenderer.GetPushConstantLocation(*pipeline, 1);
        const auto positionLocation = backendRenderer.GetPushConstantLocation(*pipeline, 2);
        const auto sizeLocation = backendRenderer.GetPushConstantLocation(*pipeline, 3);
        const auto colorLocation = backendRenderer.GetPushConstantLocation(*pipeline, 4);

        return ColoredQuad{
            .pipeline = std::move(pipeline),
            .vertexBuffer = std::move(vertexBuffer),
            .indexBuffer = std::move(indexBuffer),
            .projectionLocation = projectionLocation,
            .positionLocation = positionLocation,
            .sizeLocation = sizeLocation,
            .colorLocation = colorLocation
        };
    }

}

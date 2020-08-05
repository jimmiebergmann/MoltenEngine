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

#include "Molten/Gui/GuiRenderer.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Gui
{

    Renderer::Renderer() :
        m_backendRenderer(nullptr)
    {}

    Renderer::~Renderer()
    {
        Close();
    }

    void Renderer::Open(Molten::Renderer* backendRenderer)
    {
        Close();

        m_backendRenderer = backendRenderer;
        m_projection = Matrix4x4f32::Orthographic(0.0f, 800.0f, 0.0f, 600.0f, 1.0f, -1.0f);

        //LoadRectRenderInstance();
    }

    void Renderer::Close()
    {
        if (!m_backendRenderer)
        {
            return;
        }

        //DestroyRenderInstance(m_rect);
        m_backendRenderer = nullptr;
    }

    void Renderer::DrawRect(const Vector2f32& /*position*/, const Vector2f32& /*size*/, const Vector4f32& /*color*/)
    {
        /*m_backendRenderer->BindPipeline(m_rect.pipeline);

        m_backendRenderer->PushShaderConstants(Shader::Type::Fragment,  0,                    sizeof(m_projection), &m_projection);
        m_backendRenderer->PushShaderConstants(Shader::Type::Vertex,    0,                    sizeof(m_projection), &m_projection);
        m_backendRenderer->PushShaderConstants(Shader::Type::Fragment,  sizeof(m_projection), sizeof(color),        &color);
        m_backendRenderer->PushShaderConstants(Shader::Type::Vertex,    sizeof(m_projection), sizeof(color),        &color);

        m_backendRenderer->DrawVertexBuffer(m_rect.indexBuffer, m_rect.vertexBuffer);*/
    }

    Renderer::RenderInstance::RenderInstance() :
        pipeline(nullptr),
        vertexBuffer(nullptr),
        indexBuffer(nullptr),
        vertexScript(nullptr),
        fragmentScript(nullptr),
        vertexStage(nullptr),
        fragmentStage(nullptr)
    { }

    void Renderer::LoadRectRenderInstance()
    {
       /* m_rect.vertexScript = new Shader::VertexScript();
        m_rect.fragmentScript = new Shader::FragmentScript();

        {
            auto& script = *m_rect.vertexScript;

            auto outPos = script.GetVertexOutputVariable();

            auto& inputs = script.GetInputInterface();
            auto inPos = inputs.AddMember<Vector3f32>();

            auto& pushConstants = script.GetPushConstantInterface();
            auto inProjection = pushConstants.AddMember<Matrix4x4f32>();
            pushConstants.AddMember<Vector4f32>();
           

            auto inPosVec4 = script.CreateFunctionNode<Shader::Function::Vec3ToVec4f32>();
            inPosVec4->GetInputPin(0)->Connect(*inPos->GetOutputPin());
            static_cast<Shader::InputPin<float>*>(inPosVec4->GetInputPin(1))->SetDefaultValue(1.0f);

            auto finalPos = script.CreateOperatorNode<Shader::Operator::MultMat4Vec4f32>();
            finalPos->GetInputPin(0)->Connect(*inProjection->GetOutputPin());
            finalPos->GetInputPin(1)->Connect(*inPosVec4->GetOutputPin());

            outPos->GetInputPin()->Connect(*finalPos->GetOutputPin());
        }
        {
            auto& script = *m_rect.fragmentScript;

            auto& pushConstants = script.GetPushConstantInterface();
            pushConstants.AddMember<Matrix4x4f32>();
            auto inColor = pushConstants.AddMember<Vector4f32>();

            auto& outputs = script.GetOutputInterface();
            auto outColor = outputs.AddMember<Vector4f32>();

            outColor->GetInputPin()->Connect(*inColor->GetOutputPin());
        }

        m_rect.vertexStage = m_backendRenderer->CreateVertexShaderStage(*m_rect.vertexScript);
        m_rect.fragmentStage = m_backendRenderer->CreateFragmentShaderStage(*m_rect.fragmentScript);

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
        pipelineDesc.fragmentStage = m_rect.fragmentStage;
        pipelineDesc.vertexStage = m_rect.vertexStage;
        m_rect.pipeline = m_backendRenderer->CreatePipeline(pipelineDesc);*/
    }

    void Renderer::DestroyRenderInstance(RenderInstance& instance)
    {
        if (instance.pipeline)
        {
            m_backendRenderer->DestroyPipeline(instance.pipeline);
            instance.pipeline = nullptr;
        }
        if (instance.vertexStage)
        {
            m_backendRenderer->DestroyVertexShaderStage(instance.vertexStage);
            instance.vertexStage = nullptr;
        }
        if (instance.fragmentStage)
        {
            m_backendRenderer->DestroyFragmentShaderStage(instance.fragmentStage);
            instance.fragmentStage = nullptr;
        }
        if (instance.vertexBuffer)
        {
            m_backendRenderer->DestroyVertexBuffer(instance.vertexBuffer);
            instance.vertexBuffer = nullptr;
        }
        if (instance.indexBuffer)
        {
            m_backendRenderer->DestroyIndexBuffer(instance.indexBuffer);
            instance.indexBuffer = nullptr;
        }

        delete instance.vertexScript;
        instance.vertexScript = nullptr;
        delete instance.fragmentScript;
        instance.fragmentScript = nullptr;
    }

}
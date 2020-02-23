#include "Curse/Window/Window.hpp"

#include "Curse/Gui/GuiCanvas.hpp"
#include "Curse/Gui/Control/GuiButtonControl.hpp"

#include "Curse/Renderer/Renderer.hpp"
#include "Curse/Renderer/Shader/ShaderScript.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"
#include "Curse/Memory/Pointer.hpp"
#include "Curse/System/Clock.hpp"
#include "Curse/Logger.hpp"
#include <iostream>

#include <Curse/Renderer/Shader/Generator/VulkanShaderGenerator.hpp>

static Curse::Logger logger;
static Curse::Window* window = nullptr;
static Curse::Vector3f32 cameraPos = {0.0f, 0.0f, 3.0f};
static const float cameraSpeed = 1.0f;

struct UniformBuffer
{
    Curse::Shader::PaddedType<Curse::Matrix4x4f32> projViewMatrix;
    Curse::Shader::PaddedType<Curse::Matrix4x4f32> modelMatrix;
};

static void LoadShaders(Curse::Shader::VertexScript& vScript, Curse::Shader::FragmentScript& fScript)
{
    // Vertex script.
    {
        auto& script = vScript;

        auto& inBlock = script.GetInputBlock();
        auto inPos   = inBlock.AppendNode<Curse::Vector3f32>();
        auto inColor = inBlock.AppendNode<Curse::Vector4f32>();

        auto& outBlock = script.GetOutputBlock();
        auto outColor = outBlock.AppendNode<Curse::Vector4f32>();
        auto outPos   = script.GetVertexOutputNode();

        auto uBlock0 = script.CreateUniformBlock(0);
        auto uProjView = uBlock0->AppendNode<Curse::Matrix4x4f32>();
        auto uModel    = uBlock0->AppendNode<Curse::Matrix4x4f32>();
        
        auto inPosVec4 = script.CreateFunctionNode<Curse::Shader::Function::Vec3ToVec4f32>();
        inPosVec4->GetInputPin(0)->Connect(*inPos->GetOutputPin());
        static_cast<Curse::Shader::InputPin<float>*>(inPosVec4->GetInputPin(1))->SetDefaultValue(1.0f);

        auto projModelmat = script.CreateOperatorNode<Curse::Shader::Operator::MultMat4f32>();
        projModelmat->GetInputPin(0)->Connect(*uProjView->GetOutputPin());
        projModelmat->GetInputPin(1)->Connect(*uModel->GetOutputPin());

        auto finalPos = script.CreateOperatorNode<Curse::Shader::Operator::MultMat4Vec4f32>();
        finalPos->GetInputPin(0)->Connect(*projModelmat->GetOutputPin());
        finalPos->GetInputPin(1)->Connect(*inPosVec4->GetOutputPin());

        outPos->GetInputPin()->Connect(*finalPos->GetOutputPin());
        outColor->GetInputPin()->Connect(*inColor->GetOutputPin());
    }
    // Fragment script
    {
        auto& script = fScript;

        auto& inBlock = script.GetInputBlock();
        auto inColor = inBlock.AppendNode<Curse::Vector4f32>();

        auto& outBlock = script.GetOutputBlock();
        auto outColor = outBlock.AppendNode<Curse::Vector4f32>();

        auto mult = script.CreateOperatorNode<Curse::Shader::Operator::MultVec4f32>();
        auto add = script.CreateOperatorNode<Curse::Shader::Operator::AddVec4f32>();
        auto const1 = script.CreateConstantNode<Curse::Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
        auto const2 = script.CreateConstantNode<Curse::Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
        auto cos = script.CreateFunctionNode<Curse::Shader::Function::CosVec4f32>();

        outColor->GetInputPin()->Connect(*add->GetOutputPin());
        add->GetInputPin(0)->Connect(*mult->GetOutputPin());
        add->GetInputPin(1)->Connect(*const1->GetOutputPin());
        mult->GetInputPin(0)->Connect(*inColor->GetOutputPin());
        mult->GetInputPin(1)->Connect(*cos->GetOutputPin());
        cos->GetInputPin()->Connect(*const2->GetOutputPin());
    }
}

void LoadGui(Curse::Gui::Canvas& canvas)
{
    auto& plane = canvas.GetPlane();

    auto button = new Curse::Gui::Button;
    button->SetPosiion({100.0f, 200.0f});
    button->SetSize({ 30.0f, 50.0f });

    plane.Add(*button);
}

static void Run()
{
    window = Curse::Window::Create();
    Curse::Vector2ui32 windowSize = { 800, 600 };
    std::string windowTitle = "Curse Editor";
    if (!window->Open(windowTitle, windowSize, &logger))
    {
        return;
    }

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), &logger);

    Curse::Gui::Canvas canvas(*renderer, windowSize);
    LoadGui(canvas);

    Curse::Shader::VertexScript vertexScript;
    Curse::Shader::FragmentScript fragmentScript;
    LoadShaders(vertexScript, fragmentScript);

    auto vertexVec = Curse::Shader::VulkanGenerator::GenerateGlsl(vertexScript);
    std::string vertexStr(vertexVec.begin(), vertexVec.end());

    Curse::Shader::VertexStage* vertexStage = renderer->CreateVertexShaderStage(vertexScript);
    Curse::Shader::FragmentStage* fragmentStage = renderer->CreateFragmentShaderStage(fragmentScript);

    struct Vertex
    {
        Curse::Vector3f32 position;
        Curse::Vector4f32 color;
    };

    static const uint32_t vertexCount = 4;
    static const Vertex vertices[vertexCount] =
    {
        { { -0.5f, -0.5, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.5f, -0.5, 0.0f },  { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5, 0.0f },   { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f } }
    };

    static const uint32_t indexCount = 6;
    static const uint16_t indices[indexCount] =
    {
        0, 1, 2, 0, 2, 3
    };

    Curse::VertexBufferDescriptor vertexBufferDesc;
    vertexBufferDesc.vertexCount =  vertexCount;
    vertexBufferDesc.vertexSize = sizeof(Vertex);
    vertexBufferDesc.data = static_cast<const void*>(&vertices[0]);
    Curse::VertexBuffer* vertexBuffer = renderer->CreateVertexBuffer(vertexBufferDesc);

    Curse::IndexBufferDescriptor indexBufferDesc;
    indexBufferDesc.indexCount = indexCount;
    indexBufferDesc.data = static_cast<const void*>(indices); 
    indexBufferDesc.dataType = Curse::IndexBuffer::DataType::Uint16;
    Curse::IndexBuffer* indexBuffer = renderer->CreateIndexBuffer(indexBufferDesc);

    Curse::PipelineDescriptor pipelineDesc;
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;
    pipelineDesc.vertexStage = vertexStage;
    pipelineDesc.fragmentStage = fragmentStage;

    Curse::Pipeline* pipeline = renderer->CreatePipeline(pipelineDesc);
    
    Curse::UniformBufferDescriptor uniformBufferDesc;
    uniformBufferDesc.size = 512;
    Curse::UniformBuffer* uniformBuffer = renderer->CreateUniformBuffer(uniformBufferDesc);

    Curse::UniformBlockDescriptor uniformBlockDesc;
    uniformBlockDesc.id = 0;
    uniformBlockDesc.buffer = uniformBuffer;
    uniformBlockDesc.pipeline = pipeline;
    Curse::UniformBlock* uniformBlock = renderer->CreateUniformBlock(uniformBlockDesc);

    /*Curse::UniformBlockDescriptor uniformBlockDesc2;
    uniformBlockDesc2.id = 1;
    uniformBlockDesc2.buffer = uniformBuffer;
    uniformBlockDesc2.pipeline = pipeline;
    Curse::UniformBlock* uniformBlock2 = renderer->CreateUniformBlock(uniformBlockDesc2);*/


    auto renderFunction = [&]()
    {     
        static Curse::Clock runTimer;
        float runTime = runTimer.GetTime().AsSeconds<float>();

        static Curse::Clock fpsTimer;
        static uint32_t fps = 0;
        fps++;
        if (fpsTimer.GetTime() >= Curse::Seconds(1.0f))
        {
            fpsTimer.Reset();
            window->SetTitle(windowTitle + " - " + std::to_string(fps) + "FPS");
            fps = 0;
        }
        
        renderer->Resize(window->GetSize());
        renderer->BeginDraw();

        renderer->BindPipeline(pipeline);

        float fov = static_cast<float>(window->GetSize().x) / static_cast<float>(window->GetSize().y);
        auto projViewMatrix = Curse::Matrix4x4f32::Perspective(60.0f, fov, 0.1f, 100.0f) *
                              Curse::Matrix4x4f32::LookAtPoint(cameraPos, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

        UniformBuffer bufferData1;
        bufferData1.projViewMatrix = projViewMatrix;
        bufferData1.modelMatrix = Curse::Matrix4x4f32::Identity();
        bufferData1.modelMatrix.Translate({ std::sin(runTime * 3.0f) * 0.25f, 0.0f, 0.2f });

        UniformBuffer bufferData2;
        bufferData2.projViewMatrix = projViewMatrix;
        bufferData2.modelMatrix = Curse::Matrix4x4f32::Identity();
        bufferData2.modelMatrix.Translate({ 0.0f, std::cos(runTime * 3.0f) * 0.25f, 0.0f });

        renderer->UpdateUniformBuffer(uniformBuffer, 0, sizeof(UniformBuffer), &bufferData1);
        renderer->UpdateUniformBuffer(uniformBuffer, 256, sizeof(UniformBuffer), &bufferData2);

        renderer->BindUniformBlock(uniformBlock, 0);
        renderer->DrawVertexBuffer(indexBuffer, vertexBuffer);

        renderer->BindUniformBlock(uniformBlock, 256);
        renderer->DrawVertexBuffer(indexBuffer, vertexBuffer);

        canvas.Draw();

        renderer->EndDraw();
    };

    auto resizeCallback = [&](Curse::Vector2ui32 )
    {
        renderFunction();
    };

    window->OnMaximize.Connect(resizeCallback);
    window->OnMinimize.Connect(resizeCallback);
    window->OnResize.Connect(resizeCallback);
   
    window->Show();

    Curse::Clock deltaTimer;

    while (window->IsOpen())
    {
        const float deltaTime = deltaTimer.GetTime().AsSeconds<float>();
        deltaTimer.Reset();

        window->Update();
        if (!window->IsOpen())
        {
            break;
        }

        Curse::UserInput userInput = window->GetUserInput();
        Curse::UserInput::Event event;
        while (userInput.PollEvent(event))
        {
            switch (event.type)
            {
            case Curse::UserInput::Event::Type::KeyDown:
            {
                switch (event.keyboardEvent.key)
                {
                case Curse::Keyboard::Key::A: cameraPos.x -= cameraSpeed * deltaTime; break;
                case Curse::Keyboard::Key::D: cameraPos.x += cameraSpeed * deltaTime; break;
                case Curse::Keyboard::Key::W: cameraPos.y -= cameraSpeed * deltaTime; break;
                case Curse::Keyboard::Key::S: cameraPos.y += cameraSpeed * deltaTime; break;
                case Curse::Keyboard::Key::Q: cameraPos.z -= cameraSpeed * deltaTime; break;
                case Curse::Keyboard::Key::E: cameraPos.z += cameraSpeed * deltaTime; break;
                default: break;
                }
            }
            break;
            default: break;
            }
        }

        /*Curse::UserInput::Event event;
        while (userInput.PollEvent(event))
        {
            switch (event.type)
            {
            case Curse::UserInput::Event::Type::MouseButtonPressed: logger.Write(Curse::Logger::Severity::Info,
                "Button pressed: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseButtonDown: logger.Write(Curse::Logger::Severity::Info,
                "Button down: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseButtonReleased: logger.Write(Curse::Logger::Severity::Info,
                "Button released: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseMove : logger.Write(Curse::Logger::Severity::Info,
                "Mouse move: " + std::to_string(event.mouseMoveEvent.position.x) + " " + std::to_string(event.mouseMoveEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::KeyPressed: logger.Write(Curse::Logger::Severity::Info,
                "Key pressed: " + std::to_string((int)event.keyboardEvent.key));
                break;
            case Curse::UserInput::Event::Type::KeyDown: logger.Write(Curse::Logger::Severity::Info,
                "Key down: " + std::to_string((int)event.keyboardEvent.key));
                break;
            case Curse::UserInput::Event::Type::KeyReleased: logger.Write(Curse::Logger::Severity::Info,
                "Key released: " + std::to_string((int)event.keyboardEvent.key));
                break;
            default:
                logger.Write(Curse::Logger::Severity::Info, "Unkown event.");
                break;
            }
        }*/

        renderFunction();
    }

    renderer->WaitForDevice();
    renderer->DestroyUniformBlock(uniformBlock);
    //renderer->DestroyUniformBlock(uniformBlock2);
    renderer->DestroyUniformBuffer(uniformBuffer);
    renderer->DestroyVertexBuffer(vertexBuffer);
    renderer->DestroyIndexBuffer(indexBuffer);
    renderer->DestroyVertexShaderStage(vertexStage);
    renderer->DestroyFragmentShaderStage(fragmentStage);
    renderer->DestroyPipeline(pipeline);
    delete renderer;
    delete window;
}

int main()
{
    logger.Write(Curse::Logger::Severity::Info, "Starting Curse Editor v" + CURSE_VERSION.AsString(false));

    try
    {
        Run();
    }
    catch (Curse::Exception & e)
    {
        logger.Write(Curse::Logger::Severity::Error, "Error: " + e.GetMessage());
        logger.Write(Curse::Logger::Severity::Info, "Closing Curse Editor.");
        return -1;
    }

    return 0;
}
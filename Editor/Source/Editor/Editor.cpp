#include "Curse/Window/Window.hpp"
#include "Curse/Renderer/Renderer.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"
#include "Curse/Memory/Pointer.hpp"
#include "Curse/System/Mouse.hpp"
#include "Curse/System/Clock.hpp"
#include "Curse/Logger.hpp"
#include <iostream>

static Curse::Logger logger;
static Curse::Window* window = nullptr;

static void Run()
{
    window = Curse::Window::Create();
    if (!window->Open("Curse Editor", { 800, 600 }, &logger))
    {
        return;
    }

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), &logger);

    auto verSpirvSrc = Curse::FileSystem::ReadFile("shader.vert");
    auto verSpirv = renderer->CompileShader(Curse::Shader::Format::Glsl, Curse::Shader::Type::Vertex, verSpirvSrc, Curse::Shader::Format::SpirV);

    Curse::Shader* vertexShader1 = renderer->CreateShader({ Curse::Shader::Type::Vertex, "vert1.spv" });
    Curse::Shader* fragmentShader1 = renderer->CreateShader({ Curse::Shader::Type::Fragment, "frag1.spv" });
    Curse::Shader* vertexShader2 = renderer->CreateShader({ Curse::Shader::Type::Vertex, verSpirv.data(), verSpirv.size() });
    Curse::Shader* fragmentShader2 = renderer->CreateShader({ Curse::Shader::Type::Fragment, "frag2.spv" });

    Curse::PipelineDescriptor pipelineDesc; 
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Line;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;

    pipelineDesc.shaders = { vertexShader1 , fragmentShader1 };
    Curse::Pipeline* pipeline1 = renderer->CreatePipeline(pipelineDesc);

    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.shaders = { vertexShader2 , fragmentShader1 };

    struct Vertex
    {
        Curse::Vector2f32 position;
        Curse::Vector3f32 color;
    };

    static const size_t vertexCount = 3;
    static const Vertex vertices[vertexCount] =
    {
        { { 0.0f, -0.3f }, { 1.0f, 1.0f, 1.0f } },
        { { 0.3f, 0.3f },  { 0.0f, 1.0f, 0.0f } },
        { { -0.3f, 0.3f }, { 0.0f, 0.0f, 1.0f } }
    };

    Curse::VertexBufferDescriptor vertexBufferDesc;
    vertexBufferDesc.size = sizeof(Vertex) * vertexCount;
    vertexBufferDesc.data = static_cast<const void*>(&vertices[0]);

    Curse::VertexBuffer* vertexBuffer = renderer->CreateVertexBuffer(vertexBufferDesc);

    Curse::Pipeline::VertexBinding vertexBinding;
    vertexBinding.binding = 0;
    vertexBinding.stride = sizeof(Vertex);

    Curse::Pipeline::VertexAttribute vertexAttrib1;
    vertexAttrib1.location = 0;
    vertexAttrib1.offset = offsetof(Vertex, position);
    vertexAttrib1.format = Curse::Pipeline::AttributeFormat::R32_G32_Float;
    vertexBinding.attributes.push_back(vertexAttrib1);

    Curse::Pipeline::VertexAttribute vertexAttrib2;
    vertexAttrib2.location = 1;
    vertexAttrib2.offset = offsetof(Vertex, color);
    vertexAttrib2.format = Curse::Pipeline::AttributeFormat::R32_G32_B32_Float;
    vertexBinding.attributes.push_back(vertexAttrib2);

    pipelineDesc.vertexBindings = { vertexBinding };
    Curse::Pipeline* pipeline2 = renderer->CreatePipeline(pipelineDesc);
    
    auto renderFunction = [&]()
    {      
        renderer->Resize(window->GetSize());
        renderer->BeginDraw();

        //renderer->BindPipeline(pipeline1);
        //renderer->DrawVertexArray(nullptr);

        renderer->BindPipeline(pipeline2);
        renderer->DrawVertexBuffer(vertexBuffer);

        renderer->EndDraw();
    };

    //Curse::Clock resizeTimer;
    auto resizeCallback = [&](Curse::Vector2ui32 )
    {
        /*if (resizeTimer.GetTime() >= Curse::Seconds(0.1f))
        {
            resizeTimer.Reset();
            
            //Curse::Clock rendererTimer;
            //renderFunction();
            //auto time = rendererTimer.GetTime();
            //std::cout << "Resize time: " << time.AsSeconds<float>() << std::endl;
        }*/
    };

    window->OnMaximize.Connect(resizeCallback);
    window->OnMinimize.Connect(resizeCallback);
    window->OnResize.Connect(resizeCallback);
    

    window->Show();

    Curse::Vector2f32 windowPos = window->GetPosition();

    //size_t ticks = 0;
    Curse::Clock deltaClock;
    Curse::Clock runClock;
    float deltaTime = 0.0f;
    while (window->IsOpen())
    {
        deltaTime = deltaClock.GetTime().AsSeconds<float>();
        deltaClock.Reset();
            
        /*const float resizeSpeed = 2.0f;
        const float resizeMagniture = 200.0f;
        float resize = (std::sin(runClock.GetTime().AsSeconds<float>() * resizeSpeed) + 1.0f) / 2.0f * resizeMagniture;
        auto newSize = Curse::Vector2f32{ 400.0f + resize, 400.0f + resize };
        auto newPos = Curse::Vector2f32{ 400.0f - (resize / 2.0f), 400.0f - (resize/2.0f) };
        window->Move(newPos);*/
        //window->Resize(newSize);


        //logger.Write(Curse::Logger::Severity::Debug, std::to_string(resize));

        //window->Move(windowPos);
        //window->Resize({300, 300});

        window->Update();
        if (!window->IsOpen())
        {
            break;
        }

        renderFunction();
    }

    renderer->WaitForDevice();
    renderer->DestroyVertexBuffer(vertexBuffer);
    renderer->DestroyShader(vertexShader1);
    renderer->DestroyShader(fragmentShader1);
    renderer->DestroyShader(vertexShader2);
    renderer->DestroyShader(fragmentShader2);
    renderer->DestroyPipeline(pipeline1);
    renderer->DestroyPipeline(pipeline2);
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
#include "Curse/Window/Window.hpp"
#include "Curse/Renderer/Renderer.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/Memory/Pointer.hpp"
#include "Curse/System/Mouse.hpp"
#include "Curse/System/Clock.hpp"
#include <iostream>

static int Run()
{
    auto window = Curse::Window::Create("Curse Editor", Curse::Vector2ui32(800, 600));
    
    auto debugPrinter = [](const std::string & message)
    {
        std::cout << "[Debug] Renderer: " << message << std::endl;
    };

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), debugPrinter);
    
    Curse::Shader* vertexShader1 = renderer->CreateShader({ Curse::Shader::Type::Vertex, "vert1.spv" });
    Curse::Shader* fragmentShader1 = renderer->CreateShader({ Curse::Shader::Type::Fragment, "frag1.spv" });
    Curse::Shader* vertexShader2 = renderer->CreateShader({ Curse::Shader::Type::Vertex, "vert2.spv" });
    Curse::Shader* fragmentShader2 = renderer->CreateShader({ Curse::Shader::Type::Fragment, "frag2.spv" });

    Curse::PipelineDescriptor pipelineDesc; 
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Line;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;

    pipelineDesc.shaders = { vertexShader1 , fragmentShader1 };
    Curse::Pipeline* pipeline1 = renderer->CreatePipeline(pipelineDesc);

    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.shaders = { vertexShader2 , fragmentShader2 };
    Curse::Pipeline* pipeline2 = renderer->CreatePipeline(pipelineDesc);
    
    auto renderFunction = [&]()
    {      
        renderer->Resize(window->GetCurrentSize());
        renderer->BeginDraw();

        renderer->BindPipeline(pipeline1);
        renderer->DrawVertexArray(nullptr);

        renderer->BindPipeline(pipeline2);
        renderer->DrawVertexArray(nullptr);

        renderer->EndDraw();
    };

    Curse::Clock resizeTimer;
    auto resizeCallback = [&](Curse::Vector2ui32 )
    {
        if (resizeTimer.GetTime() >= Curse::Seconds(0.1f))
        {
            resizeTimer.Reset();
            
            //Curse::Clock rendererTimer;
            renderFunction();
            //auto time = rendererTimer.GetTime();
            //std::cout << "Resize time: " << time.AsSeconds<float>() << std::endl;
        }
    };

    window->OnResize.Connect(resizeCallback);


    window->Show();

    size_t ticks = 0;
    Curse::Clock clock;
    while (window->IsOpen())
    {
        if (clock.GetTime() >= Curse::Seconds(1.0f))
        {
            //std::cout << "FPS: " << ticks << std::endl;
            ticks = 0;
            clock.Reset();
        }
        ticks++;
        
        window->Update();
        renderFunction();
    }

    renderer->DestroyShader(vertexShader1);
    renderer->DestroyShader(fragmentShader1);
    renderer->DestroyShader(vertexShader2);
    renderer->DestroyShader(fragmentShader2);
    renderer->DestroyPipeline(pipeline1);
    renderer->DestroyPipeline(pipeline2);

    return 0;
}

int main()
{
    std::cout << "Curse Editor v" << Curse::Version(CURSE_VERSION_MAJOR, CURSE_VERSION_MINOR, CURSE_VERSION_PATCH).AsString(false) << std::endl;

    try
    {
        return Run();
    }
    catch (Curse::Exception & e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return -1;
}
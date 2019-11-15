#include "Curse/Window/Window.hpp"
#include "Curse/Renderer/Renderer.hpp"
#include "Curse/System/Exception.hpp"
#include <iostream>

static int Run()
{
    Curse::Window window("", Curse::Vector2ui32(800, 600));

    auto renderer = Curse::Renderer::Create(Curse::Renderer::Type::OpenGL);
    //renderer->Open(window);

    window.Show();

    while (window.IsOpen())
    {
        window.Update();
    }

    return 0;
}

int main()
{
    std::cout << "Curse Editor v" << Curse::Version(CURSE_VERSION_MAJOR, CURSE_VERSION_MINOR, CURSE_VERSION_PATCH).AsString(false) << std::endl;

#if defined(CURSE_PLATFORM_LINUX)
    return 0;
#endif

    try
    {
        return Run();
    }
    catch (Curse::Exception & e)
    {
        std::cout << e.what() << std::endl;
    }

    return -1;
}
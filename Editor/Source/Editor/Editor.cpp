#include "Curse/Window/Window.hpp"
#include <iostream>

int main()
{
    std::cout << "Curse Editor v" << CURSE_VERSION_MAJOR << "." << CURSE_VERSION_MINOR << "." << CURSE_VERSION_PATCH << std::endl;

#if defined(CURSE_PLATFORM_LINUX)
    return 0;
#endif

    Curse::Window window("", Curse::Vector2ui32(800, 600));
    window.Show();

    while (window.IsOpen())
    {
        window.Update();
    }

    return 0;
}
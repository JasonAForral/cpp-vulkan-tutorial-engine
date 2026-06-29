#include "platform.h"

void DesktopPlatform::cleanup()
{
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

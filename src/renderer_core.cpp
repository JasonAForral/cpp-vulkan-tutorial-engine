#include "renderer.h"

Renderer::Renderer(Platform* platform)
{
    deviceExtensions = requiredDeviceExtensions;
}

Renderer::~Renderer()
{
    cleanup();
}

void Renderer::cleanup()
{
}

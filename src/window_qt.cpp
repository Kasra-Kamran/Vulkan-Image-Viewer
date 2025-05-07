#include "window_qt.hpp"
#include <vulkan/vulkan.h>

using namespace std;


VIVqtWindow::VIVqtWindow(int w, int h, string name)
{
    _window.setTitle(name.c_str());
    _window.resize(w, h);
    _window.setFlags(Qt::Window);
}

VIVqtWindow::~VIVqtWindow()
{

}

bool VIVqtWindow::shouldClose()
{
    return false;
}

void VIVqtWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR& surface)
{
    _inst.setVkInstance(instance);
    _inst.create();
    if(_inst.errorCode() != VK_SUCCESS)
    {
        throw std::runtime_error("Unknown qt error.");
    }
    _window.setSurfaceType(QSurface::VulkanSurface);
    _window.setVulkanInstance(&_inst);
    _window.create();
    surface = QVulkanInstance::surfaceForWindow(&_window);
    if(!surface)
    {
        throw std::runtime_error("Couldn't create surface.");
    }
}

VkExtent2D VIVqtWindow::getExtent() const
{
    return {800, 600};
}

bool VIVqtWindow::wasWindowResized() const 
{
    return false;
}

void VIVqtWindow::resetWindowResizedFlag()
{

}

std::vector<string> VIVqtWindow::requiredExtensions()
{
    std::vector<string> extensions;
    QVulkanInstance q;
    auto ext = q.supportedExtensions();
    for(int i = 0; i < ext.size(); i++)
    {
        extensions.push_back(string(ext[i].name.data()));
    }
    return extensions;
}

void VIVqtWindow::show()
{
    _window.show();
}
#pragma once

#include "window_base.hpp"
#include <QWindow>
#include <QVulkanInstance>
#include <vector>
#include <string>


class VIVqtWindow : public BaseWindow<VIVqtWindow>
{
public:
    VIVqtWindow(int w, int h, std::string name);
    ~VIVqtWindow();
    static std::vector<std::string> requiredExtensions();
    bool shouldClose();
    void createWindowSurface(VkInstance instance, VkSurfaceKHR& surface);
    VkExtent2D getExtent() const;
    bool wasWindowResized() const;
    void resetWindowResizedFlag();

    void show();
private:
    QWindow _window;
    QVulkanInstance _inst;
};
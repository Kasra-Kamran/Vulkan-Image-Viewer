#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <concepts>
#include <vector>
#include <glm/glm.hpp>

template <typename T>
concept BaseWindowCheckConst = requires(const T& t) {
    { t.getExtent() } -> std::convertible_to<VkExtent2D>;
    { t.wasWindowResized() } -> std::convertible_to<bool>;
};

template <typename T>
concept BaseWindowCheckNonConst = requires(T t) {
    { t.shouldClose() } -> std::convertible_to<bool>;
    { t.createWindowSurface(std::declval<VkInstance>(), std::declval<VkSurfaceKHR&>()) };
    { t.resetWindowResizedFlag() };
    { t.requiredExtensions() } -> std::convertible_to<std::vector<std::string>>;
    { t.getView() } -> std::convertible_to<glm::mat4&>;
};

template <typename T>
concept MustOverrideBaseWindow = BaseWindowCheckConst<T> && BaseWindowCheckNonConst<T>;

template <typename Derived>
class BaseWindow
{
public:
    BaseWindow()
    {
        static_assert(MustOverrideBaseWindow<Derived> && std::derived_from<Derived, BaseWindow<Derived>>, "Derived window class must override all the methods of base window class.");
    };
    virtual ~BaseWindow() = default;
    BaseWindow(const BaseWindow&) = delete;
    BaseWindow& operator=(const BaseWindow&) = delete;
    inline bool shouldClose() { return static_cast<Derived*>(this)->shouldClose(); }
    inline void createWindowSurface(VkInstance instance, VkSurfaceKHR& surface) { static_cast<Derived*>(this)->createWindowSurface(instance, surface); }
    inline VkExtent2D getExtent() const { return static_cast<Derived const *>(this)->getExtent(); }
    inline bool wasWindowResized() const { return static_cast<Derived const *>(this)->wasWindowResized(); }
    inline void resetWindowResizedFlag() { static_cast<Derived*>(this)->resetWindowResizedFlag(); }
    inline std::vector<std::string> requiredExtensions() { return static_cast<Derived*>(this)->requiredExtensions(); };
    inline glm::mat4& getView() { return static_cast<Derived*>(this)->getView(); }
    inline void drawOverlay() { static_cast<Derived*>(this)->drawOverlay(); }
};
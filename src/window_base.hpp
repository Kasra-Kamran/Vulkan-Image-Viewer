#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <concepts>
#include <vector>

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
    { T::requiredExtensions() } -> std::convertible_to<std::vector<std::string>>;
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
    BaseWindow(const BaseWindow&) = delete;
    BaseWindow& operator=(const BaseWindow&) = delete;
    inline bool shouldClose() { return static_cast<Derived*>(this)->shouldClose(); }
    inline void createWindowSurface(VkInstance instance, VkSurfaceKHR& surface) { static_cast<Derived*>(this)->createWindowSurface(instance, surface); }
    inline VkExtent2D getExtent() const { return static_cast<Derived const *>(this)->getExtent(); }
    inline bool wasWindowResized() const { return static_cast<Derived const *>(this)->wasWindowResized(); }
    inline void resetWindowResizedFlag() { static_cast<Derived*>(this)->resetWindowResizedFlag(); }
    inline static std::vector<std::string> requiredExtensions() { return Derived::requiredExtensions(); };
};
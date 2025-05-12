#pragma once

#include "window_base.hpp"

#include <QVulkanInstance>
#include <QWindow>
#include <QWidget>
#include <QObject>
#include <QtGui>
#include <QEvent>
#include <QPainter>

#include <vector>
#include <string>
#include <atomic>
#include <optional>
#include <functional>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TextureFrame
{
    int x, y, w, h;
    TextureFrame calcNew(const glm::mat4& t, int width, int height)
    {
        glm::vec4 top_left{-1, -1, 0, 1};
        glm::vec4 bottom_right{1, 1, 0, 1};
        top_left = t * top_left;
        top_left.x = (top_left.x + 1)*(width/2);
        top_left.y = (top_left.y + 1)*(height/2);
        bottom_right = t * bottom_right;
        bottom_right.x = (bottom_right.x + 1)*(width/2);
        bottom_right.y = (bottom_right.y + 1)*(height/2);

        x = top_left.x;
        y = top_left.y;
        w = bottom_right.x - top_left.x;
        h = bottom_right.y - top_left.y;
    }

    glm::vec2 whereIsPoint(int x_, int y_)
    {
        if(x_ - x >= 0 && y_ - y >= 0 && y_ - y <= h && x_ - x <= w)
        {
            return glm::vec2(x_ - x, y_ - x);
        }
        return glm::vec2(-1, -1);
    }
};

class EventHandler : public QObject {
    Q_OBJECT
public:
    EventHandler(QObject *parent) : QObject(parent){}

    void registerHandler(QEvent::Type type, std::function<void(QEvent*)> callback)
    {
        _event_handlers[type] = callback;
    }

private:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if(_event_handlers.contains(event->type()))
        {
            _event_handlers[event->type()](event);
        }
        return QObject::eventFilter(obj, event);
    }

    std::unordered_map<QEvent::Type, std::function<void(QEvent*)>> _event_handlers;
};


class VIVqtWindow : public BaseWindow<VIVqtWindow>
{
public:
    VIVqtWindow(int w, int h, const std::string& name);
    ~VIVqtWindow();
    std::vector<std::string> requiredExtensions();
    bool shouldClose();
    void createWindowSurface(VkInstance instance, VkSurfaceKHR& surface);
    VkExtent2D getExtent() const;
    bool wasWindowResized() const;
    void resetWindowResizedFlag();
    glm::mat4& getView();
    void drawOverlay();

    void show();

private:
    void register_handlers();
    int _w, _h;
    QWidget _window;
    std::optional<EventHandler> _event_handler_main_window;
    std::optional<EventHandler> _event_handler_drawing_surface;
    QWindow _drawing_surface;
    QWidget* _surface_wrapper;
    QVulkanInstance _inst;
    std::atomic<bool> _windowResized = false;
    glm::mat4 _view{1.0f};
    float _aspect_ratio;
    TextureFrame _texture_frame;
};
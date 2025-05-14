#include "window_qt.hpp"
#include <vulkan/vulkan.h>
#include <QObject>

#include <iostream>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

using namespace std;


VIVqtWindow::VIVqtWindow(int w, int h, const string& name)
    : _w(w), _h(h)
{
    _aspect_ratio = static_cast<float>(w) / static_cast<float>(h);
    _window.setWindowTitle(name.c_str());
    _surface_wrapper = QWidget::createWindowContainer(&_drawing_surface, &_window, Qt::FramelessWindowHint);
    _event_handler_main_window.emplace(&_window);
    _event_handler_drawing_surface.emplace(&_window);
    _texture_frame = TextureFrame{0, 0, w, h};

    register_handlers();
    
    _window.resize(w, h);
    _drawing_surface.resize(w, h);
    _surface_wrapper->resize(_window.size());
}

VIVqtWindow::~VIVqtWindow()
{

}

void VIVqtWindow::register_handlers()
{
    _event_handler_main_window->registerHandler(QEvent::Resize, [this](QEvent* event)
    {
        int old_surface_width = _surface_wrapper->width();
        int old_surface_height = _surface_wrapper->height();
        int a = _window.width() - _surface_wrapper->width();
        int b = _window.height() - _surface_wrapper->height();
        if(a > b * _aspect_ratio)
        {
            float h = static_cast<float>(_window.height());
            float w = h * _aspect_ratio;
            _surface_wrapper->resize({static_cast<int>(w), static_cast<int>(h)});
        }
        else
        {
            float w = static_cast<float>(_window.width());
            float h = w / _aspect_ratio;
            _surface_wrapper->resize({static_cast<int>(w), static_cast<int>(h)});
        }
        _surface_wrapper->move(_window.width() / 2 - _surface_wrapper->width() / 2, _window.height() / 2 - _surface_wrapper->height() / 2);
        _windowResized.store(true);
        _texture_frame.calcNew(_view, _surface_wrapper->width(), _surface_wrapper->height());
    });
    _event_handler_drawing_surface->registerHandler(QEvent::Wheel, [this](QEvent* event)
    {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
        int width = _surface_wrapper->width();
        int height = _surface_wrapper->height();
        if(wheel_event->modifiers() & Qt::ControlModifier)
        {
            int cursor_x = wheel_event->position().x();
            int cursor_y = wheel_event->position().y();

            if(_texture_frame.whereIsPoint(cursor_x, cursor_y).x == -1)
            {
                return;
            }

            float x_ndc = (static_cast<float>(cursor_x) / width) * 2.0f - 1.0f;
            float y_ndc = (static_cast<float>(cursor_y) / height) * 2.0f - 1.0f;

            glm::vec4 mouse_ndc(x_ndc, y_ndc, 0.0f, 1.0f);

            glm::mat4 inv_view = glm::inverse(_view);
            glm::vec4 mouse_world = inv_view * mouse_ndc;

            glm::vec3 offset(mouse_world.x, mouse_world.y, 0.0f);

            _view = glm::translate(_view, offset);

            int amount = wheel_event->angleDelta().y();
            float scale = amount * 0.01;
            if (amount < 0)
            {
                scale = 1.0 / -scale;
            }
            _view = glm::scale(_view, glm::vec3(scale));
            _view = glm::translate(_view, -offset);

            _texture_frame.calcNew(_view, width, height);
        }
        else if(wheel_event->modifiers() & Qt::ShiftModifier)
        {
            int amount = wheel_event->angleDelta().y();
            float distance = amount * 0.001;
            _view = glm::translate(_view, glm::vec3(-distance, 0, 0));
            _texture_frame.calcNew(_view, width, height);
        }
        else
        {
            int amount = wheel_event->angleDelta().y();
            float distance = amount * 0.001;
            _view = glm::translate(_view, glm::vec3(0, -distance, 0));
            _texture_frame.calcNew(_view, width, height);
        }
    });
    _event_handler_drawing_surface->registerHandler(QEvent::MouseMove, [this](QEvent* event)
    {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        static bool was_clicking = false;
        static QPoint last_pos;
        QPoint cursor_position = mouse_event->pos();
        if(mouse_event->buttons() & Qt::LeftButton && _texture_frame.whereIsPoint(cursor_position.x(), cursor_position.y()).x != -1)
        {
            if(!was_clicking)
            {
                last_pos = cursor_position;
                was_clicking = true;
                return;
            }
            QPoint delta = cursor_position - last_pos;
            last_pos = cursor_position;
            int width = _surface_wrapper->width();
            int height = _surface_wrapper->height();
            float x_ndc = (static_cast<float>(delta.x()) / width) * 2.0f;
            float y_ndc = (static_cast<float>(delta.y()) / height) * 2.0f;
            glm::vec4 translation(x_ndc, y_ndc, 0, 0);
            translation = glm::inverse(_view) * translation;
            _view = glm::translate(_view, glm::vec3(translation.x, translation.y, 0));
            _texture_frame.calcNew(_view, width, height);
        }
        else
            was_clicking = false;
    });
    _drawing_surface.installEventFilter(&*_event_handler_drawing_surface);
    _window.installEventFilter(&*_event_handler_main_window);
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
    _drawing_surface.setSurfaceType(QSurface::VulkanSurface);
    _drawing_surface.setVulkanInstance(&_inst);
    _drawing_surface.create();
    surface = QVulkanInstance::surfaceForWindow(&_drawing_surface);
    if(!surface)
    {
        throw std::runtime_error("Couldn't create surface.");
    }
}

VkExtent2D VIVqtWindow::getExtent() const
{
    return {static_cast<uint32_t>(_surface_wrapper->width()),
            static_cast<uint32_t>(_surface_wrapper->height())};
}

bool VIVqtWindow::wasWindowResized() const 
{
    return _windowResized.load();
}

void VIVqtWindow::resetWindowResizedFlag()
{
    _windowResized.store(false);
}

std::vector<string> VIVqtWindow::requiredExtensions()
{
    std::vector<string> extensions;
    auto ext = _inst.supportedExtensions();
    for(int i = 0; i < ext.size(); i++)
    {
        extensions.push_back(string(ext[i].name.data()));
    }
    return extensions;
}

void VIVqtWindow::show()
{
    _window.show();
    _surface_wrapper->show();
}

glm::mat4& VIVqtWindow::getView()
{
    return _view;
}

void VIVqtWindow::drawOverlay()
{
}
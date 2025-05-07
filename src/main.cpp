#include <vector>
#include <stdexcept>
#include <iostream>

#include <vulkan/vulkan.h>

#include <QApplication>
#include <QWindow>

#include "vivdevice.hpp"
#include "window_qt.hpp"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    VIVqtWindow w(800, 600, "app");

    VIVDevice d(w);

    w.show();

    return app.exec();
}
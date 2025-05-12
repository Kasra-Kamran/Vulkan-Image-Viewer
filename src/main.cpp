#include <vector>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

#include <vulkan/vulkan.h>

#include <QApplication>
#include <QWindow>
#include <QTimer>
#include <QFileDialog>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vivdevice.hpp"
#include "window_qt.hpp"
#include "viv.hpp"

#include <stb_image.h>


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    
    QString file_name = QFileDialog::getOpenFileName(
        nullptr,
        "Open File",
        "",
        "Images (*.bmp)(*.png)"
    );


    std::string file_name_owned(file_name.toUtf8().constData());

    int x, y, n;
    if(!stbi_info(file_name_owned.c_str(), &x, &y, &n))
    {
        throw std::runtime_error("Couldn't open image.\n");
    }
    
    
    VulkanImageViewer<VIVqtWindow> v(x, y, "Vulkan Image Viewer");

    v.importImage(file_name_owned);

    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&]()
    {
        v.drawFrame();
    });

    t.start(5);


    ((VIVqtWindow*)v.getWindow().get())->show();


    return app.exec();
}
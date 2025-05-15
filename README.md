# Vulkan-Image-Viewer


This is an image viewer, built with Vulkan and Qt6.

To build follow these steps:
```
git clone --recurse-submodules https://github.com/Kasra-Kamran/Vulkan-Image-Viewer.git
cd Vulkan-Image-Viewer
cmake . .
cmake --build .
```

## Dependencies
Qt6

cmake

A C++20 capable compiler

Python3

---

If cmake doesn't find your Qt installation directory, you can manually set it using -DCMAKE_PREFIX_PATH (e.g. -DCMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64)

On windows you might also need to add the the qt library directory (where the .dll files are) to your LIB environment variable.

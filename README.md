# Vulkan-Image-Viewer


This is an image viewer, built with Vulkan and Qt6.

To build and follow these steps:
```
git clone --recurse-submodules REPO_URL
cd REPO_FOLDER
cmake . .
make
./bin/main
```

Make sure to run the project from the root folder and not from bin, since the project will look for shaders in a subfolder named shaders from wherever you run the project.

## Dependencies
Qt6
cmake
A C++20 capable compiler

---

The project has to be built with -O0, since any compiler optimization causes the program to crash for some reason.

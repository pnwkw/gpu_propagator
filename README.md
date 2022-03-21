# Visualisation of tracks using accurate model of ALICE detector magnets

This repository contains supplemental code for poster presentation of the same title at *29th International Conference On Ultra - Relativistic Nucleus - Nucleus Collisions* - **Quark Matter 2022**.


## Installation

To build the software you need to install a C++ compiler and CMake, which can be done the easiest way by using the system package manager.

Then, clone the repository to a local directory on your system by issuing the following command:
```
git clone --recurse-submodules https://github.com/pnwkw/gpu_propagator
```
It will fetch the contents of the repository as well as code of all used libraries.

If you want to run the program on a Linux machine running in text-mode (i.e. without a graphical interface such as *Gnome* or *KDE*) you need to change the OpenGL provider used
by the program from GLFW3 to EGL.

This can be done by commenting out the `set(USE_GLFW_CONTEXT true)` and uncommenting `set(USE_EGL_CONTEXT true)` in the `CMakeLists.txt` (lines 8 and 9):
```
# EGL can be used to execute on systems with no graphics (text mode), otherwise GLFW
#set(USE_GLFW_CONTEXT true)
set(USE_EGL_CONTEXT true)
```

If using Windows or Linux with a graphical interface installed, then the default GLFW3 can be used and no additional changes are needed.

The next step is to create a build directory with:
```
cmake -B build
```
CMake will generate build scripts for you. To compile the program, type:
```
cmake -B build -S .
```

## Benchmark

First, move to the directory where the executable resides:
```
cd build
```
### Configuration
In the build directory an example `config.json` was automatically created. Here, the benchmark application can be configured to perform a specific experimental scenario.

Options:

* `samples`: The number of evaluated points in 3D space. Has to be less or equal the number of available particles in the user-supplied `events.json`.
* `method`: Controls which method of magnetic field data access will be used (*constant*, *constant_barrel*, *glsl*).
### Running
Execute the benchmark application with the following command:
```
./quark
```
The application will run and record the measurements (time spent rendering and model accuracy) to a CSV file.

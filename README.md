# Node App

## Node-Based Image Manipulation Interface

This project creates a node based interface for image manipulation using a graph like structure in C++. The app allows users to add images, process them through series of nodes such as blur or blend nodes and produces the results through the output node. This interface should function similarly to node-based editors like Substance Designer, where operations are represented visually and can be connected in various configurations.

## Features implemented

I have used libraries like glfw and imgui to create window and gui for the application. I have used the docking branch of the imgui repository so that I can implement dock space to my project.

## Build Instructions
I have used CMake and Make to build my project so you should install them first using [CMake](https://cmake.org/download/) and [Make](https://ftp.gnu.org/gnu/make/). Once installed you can use following commands in the project base directory:
```bash
mkdir build
cd build
cmake .. -G "GNU Makefiles"
```

This will install all the dependencies required for the project if required once you build the makefiles. 
``` bash
make
```
This builds the NodeApp.exe you can execute.

# Node App

## Node-Based Image Manipulation Interface

This project creates a node based interface for image manipulation using a graph like structure in C++. The app allows users to add images, process them through series of nodes such as blur or blend nodes and produces the results through the output node. This interface should function similarly to node-based editors like Substance Designer, where operations are represented visually and can be connected in various configurations. The architecture used here is mediator pattern, the core of the application holds a graph structure that creates connections between nodes and further simplifies the processing done onto the final image.

## Features implemented

I have used libraries like glfw and imgui to create window and gui for the application. I have used the docking branch of the imgui repository so that I can implement dock space to my project. I have used OpenCV to import images and edit textures. I have used OpenGL to simplify Texture generation and can make the processing of more critical nodes easier through Framebuffers.

## Functionality

![Example](assets/example.png)

The application for now is static library based and doesn't use any dynamic linking. The main component the comprises of this application is the NodeManager class which is the central hub for all processing of the graph and GUI connections. The NodeManager is a static class that persists over multiple target classes so that the data exchange can be handled efficiently. Following are the current functionalities of the node editor:

### 1. Brightness and Contrast Node
- Adjusts the overall tone and lighting of the image.

- Parameters:

    - Brightness: Adds or subtracts a constant value from pixel intensities.

    - Contrast: Scales pixel values to increase or decrease contrast.

- Real-time preview helps dial in the right balance.

### 2. Color Channel Splitter
- Decomposes input image into color components based on selected mode:

    - RGB (Red, Green, Blue)

    - Grayscale (R, G, B intensities)

    - HSV (Hue, Saturation, Value)

- Previews individual channel outputs.

### 3. Blur Node
- Applies blurring filters to the image.

- Supports Gaussian blur with configurable radius (1-20px) with option for uniform and directional blur.

- Adjustable kernel size per mode.

- Ideal for noise reduction or smoothing effects.

### 4. Threshold Node
- Applies binary, otsu's, Adapitive mean and Adaptive Gaussian thresholding to the input image.

- Adjustable Threshold value (for binary), Block Size/C (for Adaptive), displays a histogram for help.

- Works on both grayscale and color inputs (auto-converts as needed).

### 5. Convolution Filter Node
- Applies blur, sharpen, edge detection, embossing, identity filter to image nodes.
- Custom Kernel inputs, along with 3x3 and 5x5 kernel matrices.
- Visual feedback of kernel matrix and preview.

### 6. Edge Detection Node
- Implements both Sobel and Canny edge detection algorithms.
- Customizable parameters:
    - Sobel: kernel size
    - Canny: low and high thresholds
- Option to overlay detected edges onto the original image.

### 7. Blend Node
- Combines two input images using different blending modes.
- Supports configurable blend factor (alpha).
- Modes:
    - Linear interpolation
    - Multiply
    - Additive
    - Overlay
    - Difference

I have used framebuffers to make the image processing more easier however I only thought it was necessary for 3D Texture generation thus I didn't use it as of now. I plan to use them.

## Build Instructions
I have used CMake and Make to build my project so you should install them first using [CMake](https://cmake.org/download/) and [Make](https://ftp.gnu.org/gnu/make/). You have to install opencv from the official website and build it with mingw to get the static libraries.I have created a script file for the setup of the same.
```bash
python setup.py
mkdir build
cd build
cmake .. -G "GNU Makefiles"
```

This will install all the dependencies required for the project if required once you build the makefiles. 
``` bash
make
```
This builds the NodeApp.exe you can execute.


## Update Log (15-04-2025)
- Added more type of nodes for processing image. Need to handle edge cases related to graphs. Also error handling is a bit bland so gotta fix these issues. The socket structure needs to be restructured and refactored.
- Added setup.py to check dependencies. Also I am for now haven't tested opencv installation through cmake. Will add it later on.
- Updated the UI colors and style. Added new node types and fixed the double node addition issue.
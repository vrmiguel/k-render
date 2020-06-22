# k-render

k-render is a minimal software renderer written in C++11. It reads [Wavefront .OBJ](https://en.wikipedia.org/wiki/Wavefront_.obj_file) files and outputs .TGA files.

## Features

* Somewhat fast Bresenham line algorithm implementation
* Wireframe rendering
* Gouraud shading
* Z-buffering

## Usage

k-render is a command-line based application. There is one obligatory argument, `-o, --obj`, which must lead to an .OBJ file (optionally including pathname). You can also set the output file's resolution with `-w, --width` and `-h, --height`. If only one of these is supplied, a square resulting image will be implied.

#### Example usage

```
./krender --obj head.obj 
krender: height and width not set. Assuming 3200 x 3200.
krender: read model "head.obj" with 1258 vertices and 2492 faces.
krender: successfully saved "output-wireframe.tga".
krender: successfully saved "output-gouraud-no-z.tga".
krender: successfully saved "output-gourand-with-z.tga".
```

The following images were saved:

![Gouraud shading with z-buffering](https://user-images.githubusercontent.com/36349314/85306519-fbb75400-b484-11ea-964d-5b277aeb299b.png)
![Gouraud shading without z-buffering](https://user-images.githubusercontent.com/36349314/85306524-fce88100-b484-11ea-9cf2-28d4109b95a8.png)
![Wireframe](https://user-images.githubusercontent.com/36349314/85306526-fe19ae00-b484-11ea-9d62-59cdfadd2483.png)


## Build 

The only dependencies are QMake and a C++11 compiler.

Build by running
``` qmake && make ```

## About

This is a basic project made in order to learn more about computer graphics and was made following class guides from Dmitry Sokolov.

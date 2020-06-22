# k-render

k-render is a minimal software renderer written in C++11. It reads [Wavefront .OBJ](https://en.wikipedia.org/wiki/Wavefront_.obj_file) files and outputs .TGA files.

## Features

* Wireframe rendering
* Gouraud shading
* Z-buffering

## Usage

k-render is a command-line based application. There is one obligatory argument, `-o, --obj`, which must lead to an .OBJ file (optionally including pathname). You can also set the output file's resolution with `-w, --width` and `-h, --height`. If only one of these is supplied, a square resulting image will be implied.

``` ./krender --width 2600 --height 2800 --obj head.obj ```

## Build 

The only dependencies are QMake and a C++11 compiler.

Build by running
``` qmake && make ```

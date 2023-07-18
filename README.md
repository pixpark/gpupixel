GPUPixel@[PixPark](https://github.com/pixpark) 

[ENGLISH]() | [中文]()

## Intro ##

GPUPixel is a C++ version GPUImage Framework, which is an open-source project designed for performing GPU-accelerated image and video processing on Mac, iOS, and Android platforms.

It was written in C++11 and can target Mac, iOS, Android, Windows, Linux, and future platforms that support C++ and OpenGL/ES.

GPUPixel boasts exceptionally high performance, enabling it to handle real-time video or image processing with ease on desktops and mobile devices alike.

## Requirements ##

- C++11
- Mac or Win or Linux
- iOS: 10.0 or higher
- OSX: 10.9 or higher
- Linux: Support OpenGL 3.x

## Architecture ##

Like GPUimage, GPUPixel relies on the concept of a processing pipeline, where image sources are targeted at image consumers, and so on down the line until images are output to the screen, to image files, to raw data, or to recorded movies. Cameras, movies, still images, and raw data can be inputs into this pipeline. Arbitrarily complex processing operations can be built from a combination of a series of smaller operations.

## Using GPUPixel ##

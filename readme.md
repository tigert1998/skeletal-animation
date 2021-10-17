# Skeletal Animation

This repository is a demo of skeletal animation. I use [assimp](http://assimp.org/) to load `fbx` format model including its binding animation. [glm](https://glm.g-truc.net/) is applied to deal with mathematics related works, mainly about matrix transformations. Rendering is implemented with OpenGL 4.1.

## Build

News: the building system switches from premake to CMake now.
OpenGL 4.1 is used because 4.1 is the latest version MacOS supports:smile:.
Use apt to install assimp on ubuntu, brew on MacOS or other package managers.

```
# download assimp manually
python configure.py # download glad
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Screenshots

![sprite](resources/sprite.png)
# Tiger Rendering Engine

## Build
This repository follows the standard CMake building procedure.
Before building, run `configure.py` to download the `glad` package.
OpenGL 4.1 is used because 4.1 is the latest version MacOS supports.
So this repository can stay cross-platform.
The `USE_SYSTEM_ASSIMP` and `USE_SYSTEM_BULLET` flag decides whether the system library of `assimp` and `bullet3` is used.
If not able to installed these two libraries in the system path,
simplily remember to leave them turned off and let CMake build the libraries for you.

```
git submodule update --init --recursive
python configure.py # download glad
mkdir build
cd build
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_SYSTEM_ASSIMP=ON \
    -DUSE_SYSTEM_BULLET=ON \
    ..
```

## Model Visualizer
This visualizer supports skeletal animation.
Please navigate through the scene with `WASD` and `HJKL`.
`WASD` help you move forward/leftward/backward/rightward.
Press `H` or `L` to rotate counter-clockwise or clockwise.
`JK` are for moving downward and moving upward. 
Check the visualizer out!

```
make model-visualizer
```

![](https://drive.google.com/uc?export=download&id=1Vuf-17OpmJaZwIQadbFFJ6ZEOA8ye0yi)

## Water Simulation

Still under construction.
# Tiger Rendering Engine

## Build
This repository follows the standard CMake building procedure.
Before building, run `configure.py` to download the `glad` package.
OpenGL 4.1 is used because 4.1 is the latest version MacOS supports.
So this repository can stay cross-platform.

```
# download assimp manually
python configure.py # download glad
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Model Visualizer
This visualizer supports skeletal animation.
Check it out!

```
make model-visualizer
```

![](https://drive.google.com/uc?export=download&id=1Vuf-17OpmJaZwIQadbFFJ6ZEOA8ye0yi)
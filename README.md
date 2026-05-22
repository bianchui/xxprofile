# build

```bash
./build.sh
```

# xxprofile runtime dependency

only compression libraries used by the runtime:

- `libs/zlib` 
- `libs/lz4-1.9.4`: [lz4](https://github.com/lz4/lz4)
- `libs/zstd-1.5.5`: [zstd](https://github.com/facebook/zstd) [home](https://facebook.github.io/zstd/)

# xxprofile viewer dependency

The viewer is built from vendored libraries in this repository.

Normal CMake and Xcode builds should use the bundled GLFW instead of a system-installed GLFW.

- C++11 compiler
- `libs/imgui`: [Dear ImGui](https://github.com/ocornut/imgui)
- `libs/glfw-3.3.10`: [GLFW](https://www.glfw.org/): 

# include but not use

- `libs/lzma`
- `libs/lzo-2.10`: [lzo](http://www.oberhumer.com/opensource/lzo/)

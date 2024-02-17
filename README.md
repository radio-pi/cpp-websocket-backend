# radio pi backend

On a quest to rebuild the current [python-websocket-backend](https://github.com/radio-pi/python-websocket-backend/)
in CPP for fun.

A simple REST server to expose a simple API to control VLC.

Using [libvlcpp](https://github.com/videolan/libvlcpp/) &
[Crow](https://github.com/CrowCpp/Crow).

## Build

```
sudo dnf install vlc-devel libvlcpp-devel

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
make -j9
```

## Format Code

```
clang-format src
```

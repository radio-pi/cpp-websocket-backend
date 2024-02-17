# radio pi backend

## Build

```
sudo dnf install vlc-devel libvlcpp-devel

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
make -j9
```

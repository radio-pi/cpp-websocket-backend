# radio pi backend

On a quest to rebuild the current [python-websocket-backend](https://github.com/radio-pi/python-websocket-backend/)
in CPP for fun.

A simple REST server to expose a simple API to control VLC.

Using [libvlcpp](https://github.com/videolan/libvlcpp/) &
[Crow](https://github.com/CrowCpp/Crow).

## Build

Fedora

```
sudo dnf install vlc-devel libvlcpp-devel
```

Debian (on a RPI)

```
sudo apt install python3-pip
sudo pip install --break-system-packages conan
conan profile detect --force
# make sure its cpp20!

sudo apt install cmake
sudo apt install vlc libvlc-dev
wget https://code.videolan.org/videolan/libvlcpp/-/archive/master/libvlcpp-master.zip && unzip libvlcpp-master.zip
sudo cp -r libvlcpp-master/vlcpp/ /usr/include/
rm -rf libvlcpp-master libvlcpp-master.zip
```

All

```
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
make -j9
```

## Format Code

```
clang-format src
```

## Deploy systemd service

```
sudo cp radiopi.service /lib/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable radiopi
sudo systemctl start radiopi
```

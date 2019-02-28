# Adjust brightness of LG UltraFine displays

Compiles and works on Ubuntu 16.04 and 18.10

Based on these two projects:
- https://github.com/unknownzerx/lguf-brightness
- https://github.com/csujedihy/LG-Ultrafine-Brightness

## Instructions to compile and run

In order to compile this project, you will need `g++`, `cmake`, `libusb-1.0-0-dev` and `libncurses5-dev`.

### Commands to compile

* `cd build`
* `cmake ..`
* `make`

### In order to run this program, you need administrator privileges

`sudo ./lguf_brightness`

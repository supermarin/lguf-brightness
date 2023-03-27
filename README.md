# Adjust brightness of LG UltraFine displays

This fork adds support for multiple LG-Ultrafine monitors.
Tested with Linux 6.x and Nix.

Based on these two projects:
- https://github.com/unknownzerx/lguf-brightness
- https://github.com/csujedihy/LG-Ultrafine-Brightness

## The easiest way to compile is with Nix
* `nix build`
After running the above command, you can find the compile binary under ./result,
and run it directly `./result/bin/lguf-brightness`.

## [OLD] Instructions to compile and run
These should still work, but I'm not using them directly.

In order to compile this project, you will need `g++`, `cmake`, `libusb-1.0-0-dev` and `libncurses5-dev`.


### [OLD] Commands to compile

* `cd build`
* `cmake ..`
* `make`

### In order to run this program, you need administrator privileges

`sudo ./lguf_brightness`

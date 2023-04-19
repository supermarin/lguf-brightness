/*
busb_get_device_descriptor* libusb example program to list devices on the bus
* Copyright © 2007 Daniel Drake <dsd@gentoo.org>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <iostream>
#include <vector>
#include <libusb.h>

using namespace std;

// From the HID spec:
static const int HID_GET_REPORT = 0x01;
static const int HID_SET_REPORT = 0x09;
static const int HID_REPORT_TYPE_INPUT = 0x01;
static const int HID_REPORT_TYPE_OUTPUT = 0x02;
static const int HID_REPORT_TYPE_FEATURE = 0x03;

using std::vector;
const uint16_t vendor_id = 0x43e;
const uint16_t product_id = 0x9a40;
const uint16_t max_brightness = 0xd2f0;
const uint16_t min_brightness = 0x0000;

const std::vector<uint16_t> small_steps = {
    0x0000,
    0x0190, 0x01af, 0x01d2, 0x01f7,
    0x021f, 0x024a, 0x0279, 0x02ac,
    0x02e2, 0x031d, 0x035c, 0x03a1,
    0x03eb, 0x043b, 0x0491, 0x04ee,
    0x0553, 0x05c0, 0x0635, 0x06b3,
    0x073c, 0x07d0, 0x086f, 0x091b,
    0x09d5, 0x0a9d, 0x0b76, 0x0c60,
    0x0d5c, 0x0e6c, 0x0f93, 0x10d0,
    0x1227, 0x1399, 0x1529, 0x16d9,
    0x18aa, 0x1aa2, 0x1cc1, 0x1f0b,
    0x2184, 0x2430, 0x2712, 0x2a2e,
    0x2d8b, 0x312b, 0x3516, 0x3951,
    0x3de2, 0x42cf, 0x4822, 0x4de1,
    0x5415, 0x5ac8, 0x6203, 0x69d2,
    0x7240, 0x7b5a, 0x852d, 0x8fc9,
    0x9b3d, 0xa79b, 0xb4f5, 0xc35f,
    0xd2f0};

const std::vector<uint16_t> big_steps = {
    0x0000,
    0x0190, 0x021f, 0x02e2, 0x03eb,
    0x0553, 0x073c, 0x09d5, 0x0d5c,
    0x1227, 0x18aa, 0x2184, 0x2d8b,
    0x3de2, 0x5415, 0x7240, 0x9b3d,
    0xd2f0};

static vector<libusb_device*> get_lg_ultrafine_usb_device(libusb_device **devs)
{ 
    vector<libusb_device *> lgdevs;
    libusb_device *dev = NULL;
    int i = 0, j = 0;
    uint8_t path[8];

    while ((dev = devs[i++]) != NULL)
    {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0)
        {
            printf("failed to get device descriptor");
            return lgdevs;
        }

        if (desc.idVendor == vendor_id)
        {
            if (desc.idProduct == product_id) {
              lgdevs.push_back(dev);
            }
        }
    }

    for (auto d : lgdevs) {
      struct libusb_device_descriptor desc;
      int r = libusb_get_device_descriptor(d, &desc);
      std::cout << "FOUND DEVICE. Vendor: " << desc.idVendor << "Product: " << desc.idProduct << "\n";
    }
    return lgdevs;
}

uint16_t next_step(uint16_t val, const vector<uint16_t> &steps)
{
    auto start = 0;
    auto end = steps.size() - 1;
    while (start + 1 < end)
    {
        auto mid = start + (end - start) / 2;
        if (steps[mid] > val)
        {
            end = mid;
        }
        else
        {
            start = mid;
        }
    }
    return steps[end];
}

uint16_t prev_step(uint16_t val, const vector<uint16_t> &steps)
{
    auto start = 0;
    auto end = steps.size() - 1;
    while (start + 1 < end)
    {
        auto mid = start + (end - start) / 2;
        if (steps[mid] >= val)
        {
            end = mid;
        }
        else
        {
            start = mid;
        }
    }
    return steps[start];
}

uint16_t get_brightness(libusb_device_handle *handle)
{
    u_char data[8] = {0x00};
    // int res = hid_get_feature_report(handle, data, sizeof(data));
    int res = libusb_control_transfer(handle,
                                        LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                        HID_GET_REPORT, (HID_REPORT_TYPE_FEATURE<<8)|0, 1, data, sizeof(data), 0);

    if (res < 0)
    {
        printf("Unable to get brightness.\n");
        printf("libusb_control_transfer error: %s (%d)\n", libusb_error_name(res), res);
    } 
    else {
        printf("Got brightness values.\n");
        for (int i = 0; i < sizeof(data); i++) {
            printf("0x%x  ", data[i]);
        }
        printf("\n");
    }

    uint16_t val = data[0] + (data[1] << 8);
    printf("val=%d (0x%x 0x%x 0x%x)\n", val, data[0], data[1], data[2]);

    return val;
}

void set_brightness(libusb_device_handle *handle, uint16_t val)
{
    u_char data[6] = {
        u_char(val & 0x00ff),
        u_char((val >> 8) & 0x00ff), 0x00, 0x00, 0x00, 0x00 };
    // int res = hid_send_feature_report(handle, data, sizeof(data));
    int res = libusb_control_transfer(handle,
                                      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                      HID_SET_REPORT, (HID_REPORT_TYPE_FEATURE<<8)|0, 1, data, sizeof(data), 0);

    if (res < 0)
    {
        printf("Unable to set brightness.\n");
        printf("libusb_control_transfer error: %s\n", libusb_error_name(res));
    } 
}

void adjust_brighness(libusb_device_handle *handle, char delta)
{
    auto brightness = get_brightness(handle);
    printf("Current brightness = %d%4s\r", int((float(brightness) / 54000) * 100.0), " ");

    int c = delta;
    switch (c)
    {
      case '+':
        brightness = next_step(brightness, big_steps);
        set_brightness(handle, brightness);
        break;
      case '-':
        brightness = prev_step(brightness, big_steps);
        set_brightness(handle, brightness);
        break;
      case '=':
        brightness = next_step(brightness, small_steps);
        set_brightness(handle, brightness);
        break;
      case '_':
        brightness = prev_step(brightness, small_steps);
        set_brightness(handle, brightness);
        break;
      case 'h':
        brightness = max_brightness;
        set_brightness(handle, brightness);
        break;
      case 'l':
        brightness = min_brightness;
        set_brightness(handle, brightness);
        break;
      default:
        break;
    }
}

int main(int argc, char *argv[])
{
    
    if (argc != 2) { 
        printf("Usage: lgufbrightness [+|-]");
        return -1;
    }
    char delta = argv[1][0];
    printf("%c passed.", delta);

    vector<libusb_device*> lgdevs;
    libusb_device **devs;
    int r, openCode, iface = 1;
    ssize_t cnt;
    libusb_device_handle *handle;

    r = libusb_init(NULL);
#if LIBUSB_API_VERSION >= 0x01000106
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
#else
    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_WARNING);       // LIBUSB_LOG_LEVEL_DEBUG  
#endif
    if (r < 0)
    {
        printf("Unable to initialize libusb.\n");
        return r;
    }

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
    {
        printf("Unable to get USB device list (%ld).\n", cnt);
        return (int)cnt;
    }

    lgdevs = get_lg_ultrafine_usb_device(devs);
    if (lgdevs.empty())
    {
        printf("Failed to get LG screen device.\n");
        return -1;
    }


    for (auto lgdev : lgdevs) {
        openCode = libusb_open(lgdev, &handle);
        if (openCode == 0)
        {
            libusb_set_auto_detach_kernel_driver(handle, 1);
            r = libusb_claim_interface(handle, iface);
            if (r == LIBUSB_SUCCESS) {
                adjust_brighness(handle, delta);
                libusb_release_interface(handle, iface);
                libusb_attach_kernel_driver(handle, iface);
            } else {
                printf("Failed to claim interface %d. Error: %d\n", iface, r);
                printf("Error: %s\n", libusb_error_name(r));
            }

            libusb_close(handle);
        }
        else
        {
            printf("libusb_open failed and returned %d\n", openCode);
        }
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(NULL);
    return 0;
}

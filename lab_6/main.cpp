#include <libusb.h>

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

void print_device_info(libusb_device *dev) {
  libusb_device_descriptor desc;
  int r = libusb_get_device_descriptor(dev, &desc);

  libusb_device_handle *handle = nullptr;

  libusb_open(dev, &handle);

  unsigned char buffer[256];

  if (desc.iSerialNumber == 0) {
    strcpy((char *)buffer, "unknown");
  } else {
    libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buffer,
                                       sizeof(buffer));
  }
  
  libusb_close(handle);

  cout << "Class: 0x" << hex << desc.bDeviceClass << endl;
  cout << "Vendor: 0x" << desc.idVendor << endl;
  cout << "Product: 0x" << desc.idProduct << dec << endl;
  cout << "Serial: " << string((char *)buffer) << endl;
}

int main() {
  libusb_context *ctx = nullptr;
  libusb_device **devs;
  ssize_t cnt;

  libusb_init(&ctx);
  libusb_set_debug(ctx, 3);
  cnt = libusb_get_device_list(ctx, &devs);

  cout << "Found USB devices: " << cnt << "\n" << endl;

  for (int i = 0; i < cnt; i++) {
    cout << i + 1 << ": " << endl;
    print_device_info(devs[i]);
  }

  libusb_free_device_list(devs, 1);
  libusb_exit(ctx);

  return 0;
}
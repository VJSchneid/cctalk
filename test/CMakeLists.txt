find_library(LIBUSB_LIBRARY_PATH usb)
find_path(LIBUSB_INCLUDE_DIR libusb-1.0/libusb.h)

if (NOT LIBUSB_LIBRARY_PATH OR NOT LIBUSB_INCLUDE_DIR)
    message(FATAL_ERROR "failed to find libusb!")
endif()

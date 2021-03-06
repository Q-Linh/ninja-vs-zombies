/*
* Generated by SGWrapperGen - DO NOT EDIT!
*
* SwinGame wrapper for C - Arduino
*
* Wrapping sgArduino.pas
*/

#ifndef sgArduino
#define sgArduino

#include <stdint.h>

#ifndef __cplusplus
  #include <stdbool.h>
#endif

#include "Types.h"

arduino_device arduino_device_named(const char *name);
bool arduino_has_data(arduino_device dev);
unsigned char arduino_read_byte(arduino_device dev);
unsigned char arduino_read_byte_timeout(arduino_device dev, int32_t timeout);
void arduino_read_line(arduino_device dev, char *result);
void arduino_read_line_timeout(arduino_device dev, int32_t timeout, char *result);
void arduino_send_byte(arduino_device dev, unsigned char value);
void arduino_send_string(arduino_device dev, const char *value);
void arduino_send_string_line(arduino_device dev, const char *value);
arduino_device create_arduino_device(const char *port, int32_t baud);
arduino_device create_arduino_named(const char *name, const char *port, int32_t baud);
void free_arduino_device(arduino_device *dev);
bool has_arduino_device(const char *name);
void release_all_arduino_devices();
void release_arduino_device(const char *name);

#ifdef __cplusplus
// C++ overloaded functions
unsigned char arduino_read_byte(arduino_device dev, int32_t timeout);
void arduino_read_line(arduino_device dev, int32_t timeout, char *result);
arduino_device create_arduino_device(const char *name, const char *port, int32_t baud);
void free_arduino_device(arduino_device &dev);

#endif

#endif


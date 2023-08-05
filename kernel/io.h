#pragma once

#include <stdint.h>

struct io_interface {
  const char *name;
  void (*write)(uint8_t byte);
  uint8_t (*read)(void);
};

void io_puts(struct io_interface *iface, char *str);
void io_putx(struct io_interface *iface, uint32_t num);
void io_putu(struct io_interface *iface, uint32_t num);
void io_putd(struct io_interface *iface, int num);
void io_printf(struct io_interface *iface, char *fmt, ...);

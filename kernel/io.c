#include "io.h"
#include "drivers/uart1.h"
#include <stdarg.h>

void io_puts(struct io_interface *iface, char *str) {
  while (*str) {
    iface->write(*str++);
  }
}

void io_putx(struct io_interface *iface, uint32_t num) {
  if (num == 0) {
    iface->write('0');
    return;
  }

  // maximum length of a 32-bit integer in base 16 is 8 digits
  char digits[9] = {0};
  int i = 8;
  while (num > 0) {
    int digit = num % 16;
    char ch = digit < 10 ? '0' + digit : 'A' + digit - 10;
    digits[--i] = ch;
    num /= 16;
  }

  io_puts(iface, digits + i);
}

void io_putu(struct io_interface *iface, uint32_t num) {
  if (num == 0) {
    iface->write('0');
    return;
  }

  // maximum length of a 32-bit integer in base 10 is 10 digits
  char digits[11] = {0};
  int i = 10;
  while (num > 0) {
    digits[--i] = '0' + (num % 10);
    num /= 10;
  }

  io_puts(iface, digits + i);
}

void io_putd(struct io_interface *iface, int num) {
  if (num < 0) {
    iface->write('-');
    num = -num;
  }

  io_putu(iface, num);
}

void io_printf(struct io_interface *iface, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      switch (*++fmt) {
      case 'c':
        iface->write(va_arg(args, int));
        break;
      case 's':
        io_puts(iface, va_arg(args, char *));
        break;
      case 'x':
        io_putx(iface, va_arg(args, uint32_t));
        break;
      case 'u':
        io_putu(iface, va_arg(args, uint32_t));
        break;
      case 'd':
        io_putd(iface, va_arg(args, int));
        break;
      }
    } else {
      iface->write(*fmt);
    }

    fmt++;
  }

  va_end(args);
}

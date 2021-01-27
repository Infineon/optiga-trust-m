#ifndef PAL_PLATFORM_H
#define PAL_PLATFORM_H

typedef struct pal_platform_gpio
{
  void (*gpio_init)(void);
  void (*set_high)(void);
  void (*set_low)(void);
} pal_platform_gpio_t;

#endif

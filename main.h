#ifndef __MAIN_H
#define __MAIN_H

#include <stm32f3xx.h>

#define MODE_CNT (4u)

typedef void (*mode_function)(void);

typedef enum {
    MODE_WHEEL,
    MODE_ALL_ON_OFF,
    MODE_SAME_COLOR,
    MODE_ODD_EVEN
} led_mode_e;

mode_function get_mode_function(led_mode_e mode);
led_mode_e get_current_mode(void);
void set_current_mode(led_mode_e mode);

void delay(uint32_t time);

void apply_wheel_mode(void);
void apply_all_on_off_mode(void);
void apply_same_color_mode(void);
void apply_odd_even_mode(void);

void Error_Handler(void);

#endif // __MAIN_H

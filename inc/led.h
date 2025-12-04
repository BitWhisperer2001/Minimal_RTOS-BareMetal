#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

#define LED_BLUE    15   // PA15
#define LED_RED     10   // PC10
#define LED_GREEN   7    // PB7
#define LED_WHITE   2    // PC2

void init_all_led(void);
void led_off(uint8_t led_num);
void led_on(uint8_t led_num);


#endif
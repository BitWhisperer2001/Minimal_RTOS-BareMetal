#include <stdint.h>

#include "led.h"

void init_all_led(void)
{
    uint32_t *pRCC_AHB1ENR = (uint32_t*)0x40023830U;
    uint32_t *pGPIOA_MODER = (uint32_t*)0x40020000U;
    uint32_t *pGPIOB_MODER = (uint32_t*)0x40020400U;
    uint32_t *pGPIOC_MODER = (uint32_t*)0x40020800U;

    *pRCC_AHB1ENR |= (uint32_t)0x07;     // enable clock GPIOA, GPIOB, GPIOC

    *pGPIOA_MODER = 0xA8000000U;   // Reset MODE register GPIOA
    *pGPIOA_MODER &= ~(0xC0000000);
    *pGPIOA_MODER |= (uint32_t)(1 << (2 * LED_BLUE));   // Set PA15 output pin

    *pGPIOC_MODER = (uint32_t)0U;    // Reset MODE register GPIOC
    *pGPIOC_MODER |= (uint32_t)(1 << (2 * LED_RED));    // Set PC10 output pin
    *pGPIOC_MODER |= (uint32_t)(1 << (2 * LED_WHITE));  // Set PC2 output pin
    
    *pGPIOB_MODER = 0x00000280U;    // Reset MODE register GPIOB
    *pGPIOB_MODER |= (uint32_t)(1 << (2 * LED_GREEN));  // Set PB7 output pin

    led_off(LED_RED);
    led_off(LED_BLUE);
    led_off(LED_GREEN);
    led_off(LED_WHITE);
}

void led_off(uint8_t led_num)
{
    switch (led_num)
    {
        case LED_RED:
        {
            uint32_t *pGPIOC_BSRR = (uint32_t*)0x40020818U;
            *pGPIOC_BSRR &= ~(1 << LED_RED);   // clear BS
            *pGPIOC_BSRR |= (1 << 26);    // Set BR
            break;
        }
        case LED_BLUE:
        {
            uint32_t *pGPIOA_BSRR = (uint32_t*)0x40020018U;
            *pGPIOA_BSRR &= ~(1 << LED_BLUE);   // clear BS
            *pGPIOA_BSRR |= (1 << 31);    // set BR
            break;
        }
        case LED_GREEN:
        {
            uint32_t *pGPIOB_BSRR = (uint32_t*)0x40020418U;
            *pGPIOB_BSRR &= ~(1 << LED_GREEN);
            *pGPIOB_BSRR |= (1 << 23);
            break;
        }
        case LED_WHITE:
        {
            uint32_t *pGPIOC_BSRR = (uint32_t*)0x40020818U;
            *pGPIOC_BSRR &= ~(1 << LED_WHITE);
            *pGPIOC_BSRR |= (1 << 18);
            break;
        }
        default:
            break;
    }
}

void led_on(uint8_t led_num)
{
    switch (led_num)
    {
        case LED_RED:
        {
            uint32_t *pGPIOC_BSRR = (uint32_t*)0x40020818U;
            *pGPIOC_BSRR |= (1 << LED_RED);   // set BS
            break;
        }
        case LED_BLUE:
        {
            uint32_t *pGPIOA_BSRR = (uint32_t*)0x40020018U;
            *pGPIOA_BSRR |= (1 << LED_BLUE);   // set BS
            break;
        }
        case LED_GREEN:
        {
            uint32_t *pGPIOB_BSRR = (uint32_t*)0x40020418U;
            *pGPIOB_BSRR |= (1 << LED_GREEN);
            break;
        }
        case LED_WHITE:
        {
            uint32_t *pGPIOC_BSRR = (uint32_t*)0x40020818U;
            *pGPIOC_BSRR |= (1 << LED_WHITE);
            break;
        }
        default:
            break;
    }
}
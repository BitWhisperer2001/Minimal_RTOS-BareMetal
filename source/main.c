#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "mini_rtos.h"
#include "led.h"

// void stlink_en(void);

int main(void)
{
  // stlink_en();
  
  enable_processor_faults();

  init_systemtick_handler(TICK_HZ);

  init_sched_stack(SCHED_STACK_START);

  init_stack_task();

  init_all_led();

  switch_sp_to_psp();

  os_start();

  task1_handler();
}

void task1_handler(void)
{
  while(1)
  {
    led_on(LED_RED);
    task_delay(1500);
    led_off(LED_RED);
    task_delay(1500);
  }
}
void task2_handler(void)
{
  while(1)
  {
    led_on(LED_BLUE);
    task_delay(250);
    led_off(LED_BLUE);
    task_delay(250);
  }
}
void task3_handler(void)
{
  while(1)
  {
    led_on(LED_GREEN);
    task_delay(1000);
    led_off(LED_GREEN);
    task_delay(1000);
  }
}
void task4_handler(void)
{
  while(1)
  {
    led_on(LED_WHITE);
    task_delay(500);
    led_off(LED_WHITE);
    task_delay(500);
  }
}

// void stlink_en(void)
// {
//   uint32_t *pRCC_AHB1ENR = (uint32_t*)0x40023830U;
//   *pRCC_AHB1ENR |= (uint32_t)0x01;     // enable clock GPIOA

//   uint32_t *pGPIOA_MODER = (uint32_t*)0x40020000U;
//   *pGPIOA_MODER &= (uint32_t)~(0x0F << 26);
//   *pGPIOA_MODER |= (uint32_t)(0x0A << 26);

//   uint32_t *pGPIOA_TYPER = (uint32_t*)0x40020004U;
//   *pGPIOA_TYPER &= (uint32_t)~(0x03 << 13);

//   uint32_t *pGPIOA_OSSPEEDR = (uint32_t*)0x40020008U;
//   *pGPIOA_OSSPEEDR |= (uint32_t)(0x03 << 26);

//   uint32_t *pGPIOA_PUPDR = (uint32_t*)0x4002000CU;
//   *pGPIOA_PUPDR &= (uint32_t)~(0x0F << 26);
//   *pGPIOA_PUPDR |= (uint32_t)(0x09 << 26);

// }

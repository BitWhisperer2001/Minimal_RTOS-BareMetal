#ifndef _MINI_RTOS_H_
#define _MINI_RTOS_H_

#include <stdint.h>
#include "config.h"

#define DUMMY_xPSR   0x01000000U    // T-bit

#define OS_MAX_TASK     (USER_MAX_TASK + 1U)

#define INTERRUPT_DISABLE()     do{__asm volatile ("MOV R0,#0x1"); asm volatile("MSR PRIMASK,R0"); } while(0)

#define INTERRUPT_ENABLE()      do{__asm volatile ("MOV R0,#0x0"); asm volatile("MSR PRIMASK,R0"); } while(0)

typedef struct
{
  uint32_t psp_value;
  uint32_t block_count;
  uint8_t current_state;
  void (*task_hander)(void);
}Task_attribute_t;

void init_systemtick_handler(uint32_t tick_hz);

__attribute__((naked)) void init_sched_stack(uint32_t addr_top_of_stack);

void init_stack_task(void);

uint32_t get_psp_value(void);

__attribute__((naked)) void switch_sp_to_psp(void);

void enable_processor_faults(void);

void save_psp_value(uint32_t current_psp_value);

void update_next_task(void);

void task_delay(uint32_t tick_count);

void unblock_task(void);

void scheduler(void);

void os_start(void);

__attribute__((weak)) void os_task_idle_handler(void);

#endif
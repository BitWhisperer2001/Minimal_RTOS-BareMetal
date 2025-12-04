#include <stdint.h>
#include <stdio.h>

#include "mini_rtos.h"
#include "config.h"
#include "main.h"

Task_attribute_t user_task[OS_MAX_TASK];
uint8_t current_task = 1;
uint32_t g_tick_count = 0;

void SysTick_Handler(void);
__attribute__((naked)) void PendSV_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);

__attribute__((weak)) void os_task_idle_handler(void)
{
    while(1);
}

void init_systemtick_handler(uint32_t tick_hz)
{
    uint32_t *pSRVR = (uint32_t*)0xE000E014;
    uint32_t *pSCSR = (uint32_t*)0xE000E010;

    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz) - 1;

    *pSRVR &= ~(0x00FFFFFF);    // Clear value in SRVR
    *pSRVR |= count_value;      // Enter Reload value
    *pSCSR |= (uint32_t)0x06;   // Use CPU clock; Counter down
}

void os_start(void)
{
    uint32_t *pSCSR = (uint32_t*)0xE000E010;
    *pSCSR |= (uint32_t)0x01;    // Enable counter
}

__attribute__((naked)) void init_sched_stack(uint32_t addr_top_of_stack)
{
    __asm volatile("MSR MSP,%0": :  "r" (addr_top_of_stack)  :   );  // Use SP as MSP value and load addr_top_of_stack for MSP
    __asm volatile("BX LR");   // Return caller function (main)
}

void init_stack_task(void)
{
    user_task[0].current_state = TASK_READY_STATE;
    user_task[1].current_state = TASK_READY_STATE;
    user_task[2].current_state = TASK_READY_STATE;
    user_task[3].current_state = TASK_READY_STATE;
    user_task[4].current_state = TASK_READY_STATE;

    user_task[0].psp_value = IDLE_STACK_START;
    user_task[1].psp_value = T1_STACK_START;
    user_task[2].psp_value = T2_STACK_START;
    user_task[3].psp_value = T3_STACK_START;
    user_task[4].psp_value = T4_STACK_START;

    user_task[0].task_hander = os_task_idle_handler;
    user_task[1].task_hander = task1_handler;
    user_task[2].task_hander = task2_handler;
    user_task[3].task_hander = task3_handler;
    user_task[4].task_hander = task4_handler;

    uint32_t *pPSP;
    for(uint8_t i = 0; i < OS_MAX_TASK; i++)
    {
        pPSP = (uint32_t*)user_task[i].psp_value;   // SP

        pPSP--;   // xPSR
        *pPSP = DUMMY_xPSR;

        pPSP--;  // PC
        *pPSP = (uint32_t)user_task[i].task_hander;

        pPSP--;  // LR
        *pPSP = 0xFFFFFFFD;   // EXC_RETURN

        for(uint8_t j = 0; j < 13; j++)  // Enter dummy data into R0...R12
        {
            pPSP--;
            *pPSP = 0;
        }
        user_task[i].psp_value = (uint32_t)pPSP;   // Save PSP to global variable
    }
}

uint32_t get_psp_value(void)
{
    return user_task[current_task].psp_value;
}

__attribute__((naked)) void switch_sp_to_psp(void)
{
    __asm volatile("PUSH {LR}");         // This func call from main(), so LR save return instruct address in main. So we must save this value first and then jump to get_psp_value func
    __asm volatile("BL get_psp_value");  // jump to get_psp_value func
    __asm volatile("MSR PSP,R0");        // assign R0 value to PSP
    __asm volatile("POP {LR}");          // get previously saved LR value
    // enable change MSP to PSP in CONTROL register
    __asm volatile("MOV R0,#0x02");      // Save 0x02 to R0
    __asm volatile("MSR CONTROL, R0");   // write R0 valve into CONTROL register
    __asm volatile("BX LR");             // return to main()
}

void enable_processor_faults(void)
{
    uint32_t *pSHCSR = (uint32_t*)0xE000ED24;
    *pSHCSR |= (uint32_t)0x70000;  // Enable MemFault, BusFault, UsageFault
}

void save_psp_value(uint32_t current_psp_value)
{
    user_task[current_task].psp_value = current_psp_value;
}

void update_next_task(void)
{
    uint8_t state;
  
    for(uint8_t i = 0; i < OS_MAX_TASK; i++)
    {
        current_task++;
        current_task %= OS_MAX_TASK;
        state = user_task[current_task].current_state;
        if((state == TASK_READY_STATE) && (current_task != 0))
        break;
    }

    if(state != TASK_READY_STATE)
        current_task = 0;
}

void task_delay(uint32_t tick_count)
{
    INTERRUPT_DISABLE();
    user_task[current_task].block_count = tick_count + g_tick_count;
    user_task[current_task].current_state = TASK_BLOCK_STATE;
    scheduler();
    INTERRUPT_ENABLE();
}

void unblock_task(void)
{
    for(uint8_t i = 1; i < OS_MAX_TASK; i++)
    {
        if(user_task[i].current_state != TASK_READY_STATE)
        {
            if(user_task[i].block_count <= g_tick_count)
                user_task[i].current_state = TASK_READY_STATE;
        }
    }
}

void scheduler(void)
{
    uint32_t *pICSR = (uint32_t*)0xE000ED04;

    // Generate PendSV exception
    *pICSR |= (1 << 28);
}

void SysTick_Handler(void)
{
    g_tick_count++;
    unblock_task();

    uint32_t *pICSR = (uint32_t*)0xE000ED04;
    // Generate PendSV exception
    *pICSR |= (1 << 28);
}

__attribute__((naked)) void PendSV_Handler(void)
{
    /* Save context */
    // 1. Get current running task PSP
    __asm volatile("MRS R0,PSP");
    // 2. Use PSP to store R4...R11
    __asm volatile("STMDB R0!, {R4-R11}");  
    // 3. Save PSP value
    __asm volatile("PUSH {LR}");
    __asm volatile("BL save_psp_value");

    /* Retrieve context task */
    // 1. Decide next task to run
    __asm volatile("BL update_next_task");
    // 2. Get PSP value
    __asm volatile("BL get_psp_value");
    // 3. Using PSP value to retrieve R4...R11
    __asm volatile ("LDMIA R0!,{R4-R11}");
    // 4. update PSP
    __asm volatile("MSR PSP,R0");
    // 5. Return and run task
    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

void HardFault_Handler(void)
{
	printf("Exception : Hardfault\n");
	while(1);
}

void MemManage_Handler(void)
{
	printf("Exception : MemManage\n");
	while(1);
}

void BusFault_Handler(void)
{
	printf("Exception : BusFault\n");
	while(1);
}

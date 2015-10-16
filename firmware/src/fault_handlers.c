/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include "hal.h"

#ifndef TESTING

void mem_manage_handler(void) __attribute__ (( naked ));
void bus_fault_handler(void) __attribute__ (( naked ));
void hard_fault_handler(void) __attribute__ (( naked ));
void usage_fault_handler(void) __attribute__ (( naked ));

void  __attribute__ ((noinline)) pop_registers_from_fault_stack(unsigned int * hardfault_args);

/* Uncomment if GCC complains about set but not used variables */
#if 0
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

void  __attribute__ ((noinline)) pop_registers_from_fault_stack(unsigned int * hardfault_args)
{
volatile unsigned int stacked_r0;
volatile unsigned int stacked_r1;
volatile unsigned int stacked_r2;
volatile unsigned int stacked_r3;
volatile unsigned int stacked_r12;
volatile unsigned int stacked_lr;
volatile unsigned int stacked_pc;
volatile unsigned int stacked_psr;
volatile unsigned long icsr;
volatile unsigned long shcrs;
volatile unsigned long cfsr;
volatile unsigned char mmfsr;
volatile unsigned long mmar;
volatile unsigned long bfar;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	icsr = *(unsigned long*)0xE000ED04;
	shcrs = *(unsigned long*)0xE000ED24;
	cfsr = *(unsigned long*)0xE000ED28;
	mmfsr = *(unsigned char*)0xE000ED28;
	mmar = *(unsigned long*)0xE000ED34;
	bfar = *(unsigned long*)0xE000ED38;

	(void) stacked_r0;
	(void) stacked_r1;
	(void) stacked_r2;
	(void) stacked_r3;
	(void) stacked_r12;
	(void) stacked_lr;
	(void) stacked_pc;
	(void) stacked_psr;
	(void) icsr;
	(void) shcrs;
	(void) cfsr;
	(void) mmfsr;
	(void) mmar;
	(void) bfar;

	__asm("ISB");

	/* Inspect stacked_pc to locate the offending instruction. */
	for( ;; );
}
#if 0
#pragma GCC diagnostic pop
#endif


void mem_manage_handler(void)
{
	__asm volatile
	(
		" tst lr, #4										\n"
		" ite eq											\n"
		" mrseq r0, msp										\n"
		" mrsne r0, psp										\n"
		" ldr r1, [r0, #24]									\n"
		" ldr r2, handler2_address_const						\n"
		" bx r2												\n"
		" handler2_address_const: .word pop_registers_from_fault_stack	\n"
	);
}

void bus_fault_handler(void)
{
	__asm volatile
	(
		" tst lr, #4										\n"
		" ite eq											\n"
		" mrseq r0, msp										\n"
		" mrsne r0, psp										\n"
		" ldr r1, [r0, #24]									\n"
		" ldr r2, handler3_address_const						\n"
		" bx r2												\n"
		" handler3_address_const: .word pop_registers_from_fault_stack	\n"
	);
}

void hard_fault_handler(void)
{
	__asm volatile
	(
		" tst lr, #4										\n"
		" ite eq											\n"
		" mrseq r0, msp										\n"
		" mrsne r0, psp										\n"
		" ldr r1, [r0, #24]									\n"
		" ldr r2, handler8_address_const						\n"
		" bx r2												\n"
		" handler8_address_const: .word pop_registers_from_fault_stack	\n"
	);
}


void usage_fault_handler(void)
{
	__asm volatile
	(
		" tst lr, #4										\n"
		" ite eq											\n"
		" mrseq r0, msp										\n"
		" mrsne r0, psp										\n"
		" ldr r1, [r0, #24]									\n"
		" ldr r2, handler4_address_const						\n"
		" bx r2												\n"
		" handler4_address_const: .word pop_registers_from_fault_stack	\n"
	);
}

#endif

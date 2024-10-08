#ifndef __SysTick__
#define __SysTick__
#include"ConfigGPIO.h"
#define SYST_CSR   (volatile unsigned int*)0xE000E010 // SysTick Control and Status Register
#define SYST_RVR   (volatile unsigned int*)0xE000E014 // SysTick Reload Value Register
#define SYST_CVR   (volatile unsigned int*)0xE000E018 // SysTick Current Value Register
#define SYST_CALIB (volatile unsigned int*)0xE000E01C // SysTick Calibration Value Register
#define SYSTEM_CLOCK 48000000u

extern const unsigned char digitSegmentCodes[10];
extern const unsigned char greeting[8];

void SysTick_Init();
void SysTick_Handler(void);
void delay_ms(unsigned int milliseconds);

#endif

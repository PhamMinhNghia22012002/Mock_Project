#ifndef __ADC_h__
#define __ADC_h__

#include"ConfigGPIO.h"

#define 	PCC_ADC0 						0xEC
#define 	PCC_BASE            0x40065000u
#define		PCC_ADC0_INDEX 	(*(volatile unsigned int *)(0x40065000u + PCC_ADC0))
	
#define 	ADC0_BASE						0x4003B000
#define 	ADC1_BASE						0x40027000
#define		SC1									0x00
#define   CFG1								0x40
#define 	CFG2								0x44
#define 	SC2									0x90
#define 	SC3									0x94
#define 	R0									0x48

#define   ADC0_SC1						(*(volatile unsigned int *)(ADC0_BASE + SC1))
#define   ADC0_CFG1						(*(volatile unsigned int *)(ADC0_BASE + CFG1))
#define   ADC0_CFG2						(*(volatile unsigned int *)(ADC0_BASE + CFG2))
#define   ADC0_SC2						(*(volatile unsigned int *)(ADC0_BASE + SC2))
#define   ADC0_SC3						(*(volatile unsigned int *)(ADC0_BASE + SC3))
#define   ADC0_R0							(*(volatile unsigned int *)(ADC0_BASE + R0))


/* define register */
#define SCG_BASE           0x40064000u
#define SCG_FIRCDIV2			(volatile unsigned int *)(SCG_BASE + 0x304u)
	
/* Function Prototype */
void ADC_Init();	
void ADC_HWtrigger_Init();
void Convert_ADC_Channel(unsigned int adcChannel);
unsigned int Read_ADC_Result(void);

#endif

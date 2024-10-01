#ifndef __LPIT__
#define __LPIT__

#define PCC_LPIT 0xDC
#define PCC_LPIT_PCS (*(volatile unsigned int *)(0x40065000u + PCC_LPIT))
	
#define MCR 0x08
#define MSR 0x0C
#define MIER 0x10
#define LPIT_MCR (*(volatile unsigned int *)(0x40037000u + MCR))
#define LPIT_MSR (*(volatile unsigned int *)(0x40037000u + MSR))
#define LPIT_MIER (*(volatile unsigned int *)(0x40037000u + MIER))

#define TVAL0 0x20
#define CVAL0 0x24
#define TCTRL0 0x28
#define LPIT_TCTRL0 (*(volatile unsigned int *)(0x40037000u + TCTRL0))
#define LPIT_CVAL0 (*(volatile unsigned int *)(0x40037000u + CVAL0))
#define LPIT_TVAL0 (*(volatile unsigned int *)(0x40037000u + TVAL0))

// define register
#define SCG_BASE           0x40064000u
#define SCG_FIRCDIV2			(volatile unsigned int *)(SCG_BASE + 0x304u)

int isLeapYear(int year);
int getDaysInMonth(int month, int year);
void LPIT_Init_Interrupt();
void LPIT_Init(unsigned timeLoad);

#endif

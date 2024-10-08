#include"SysTick.h"
volatile unsigned int g_CountStick = 0;

/*************************************************************************************
* @description Initializes the SysTick timer to generate interrupts every 1 millisecond.
*              Configures the reload value, selects the processor clock as the clock source,
*              enables SysTick interrupts, and starts the SysTick timer.
* @param       None.
***************************************************************************************/
void SysTick_Init() 
{
	// Disable SysTick
	setClearBit(SYST_CSR, 0, RESET_BIT);
	
	// Set reload register (1ms interval)
	*SYST_RVR = SYSTEM_CLOCK / 1000 - 1;
	
	// Reset the SysTick counter value
	*SYST_CVR = 0;
	
	// Select clock source (Processor Clock)
	setClearBit(SYST_CSR, 2, SET_BIT); // Clock source, 1 = processor clock, 0 = external reference clock

	// Enable interrupt
	setClearBit(SYST_CSR, 1, SET_BIT); 
	
	// Enable SysTick
	setClearBit(SYST_CSR, 0, SET_BIT);
}

/*************************************************************************************
* @description SysTick interrupt handler, which increments the global millisecond counter 
*              (`g_CountStick`) every time the SysTick interrupt occurs.
* @param       None.
***************************************************************************************/
void SysTick_Handler(void) 
{
  g_CountStick++;   
}

/*************************************************************************************
* @description Delays the program execution for a specified number of milliseconds 
*              using the global millisecond counter (`g_CountStick`). The function compares
*              the starting tick count to the current tick count to achieve the delay.
* @param milliseconds  The number of milliseconds to delay.
***************************************************************************************/
void delay_ms(unsigned int milliseconds) 
{
	unsigned int startTicks = g_CountStick;
	while ((g_CountStick - startTicks) < milliseconds) 
	{
			// Wait until the desired delay has elapsed
	}
}

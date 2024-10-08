#include"LPIT.h"

/*************************************************************************************
* @description Helper function to determine if a given year is a leap year. A leap year 
*              occurs if the year is divisible by 4 but not by 100, or if it is divisible
*              by 400.
* @param year  The year to check.
* @return      1 if the year is a leap year, 0 otherwise.
***************************************************************************************/
int isLeapYear(int year)
{
	return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

/*************************************************************************************
* @description Returns the number of days in the given month for a specified year. 
*              February has 29 days in a leap year and 28 days otherwise. The function 
*              checks if the year is a leap year when determining the days in February.
* @param month  The month to check (1-12).
* @param year   The year to check, used to determine if it's a leap year for February.
* @return       The number of days in the specified month.
***************************************************************************************/
int getDaysInMonth(int month, int year)
{
	switch(month)
	{
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
				return 31;  // Months with 31 days
		case 4: case 6: case 9: case 11:
				return 30;  // Months with 30 days
		case 2:
				return isLeapYear(year) ? 29 : 28;  // February: 28 days, or 29 in leap years
		default:
				return 30;  // Fallback, should not happen
	}
}

/*************************************************************************************
* @description Initializes the LPIT (Low Power Interval Timer) with a specified timeout 
*              period. It configures the clock source, enables the clock for LPIT, and 
*              loads the timeout value for channel 0.
* @param timeLoad  The timeout period to load into the LPIT channel 0 timer register.
***************************************************************************************/
void LPIT_Init(unsigned timeLoad)
{
		// 111 -> LPO128
	PCC_LPIT_PCS |= (7 << 24);
	// Enable PCC clock
	PCC_LPIT_PCS |= (1 << 30);
	
	// Configure LPIT MCR
	LPIT_MCR |= (1 << 0) | (1 << 3); //1001 -> Debug Enable Bit and Module Enable clock	
	LPIT_MIER |= (1 << 0); // Enable interrupt channel 0
	
	// Configure LPIT TCTRL2
	LPIT_TCTRL0 &= ~(3 << 2);//reset 2th and 3th to use 32-bit Periodic Counter	
		// load staring value for LPIT
	LPIT_TVAL0 = timeLoad; /* timeout period for 100ms */
	
	LPIT_TCTRL0 |= (1 << 0); // Timer Channel is enabled 
}

/*************************************************************************************
* @description Initializes the interrupt for the LPIT Timer 0 by configuring the 
*              Nested Vectored Interrupt Controller (NVIC). This enables the interrupt 
*              service routine to handle the LPIT Timer 0 interrupt.
* @param       None.
***************************************************************************************/
void LPIT_Init_Interrupt()
{
	// Setting NVIC
	volatile unsigned int* NVIC_ISER = (volatile unsigned int *)(0xE000E100u + 4*(48/32));
	*NVIC_ISER |= (1 << (48 % 32)); // Enable interrupt for LPIT Timer 0
}
/*! end line !*/

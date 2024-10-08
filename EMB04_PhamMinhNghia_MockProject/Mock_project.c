#include"Mock_project.h"

/* variable for count the value of datetime */
volatile unsigned int g_CountSecond = 0;
volatile unsigned int g_Second = 10;
volatile unsigned int g_Minute = 45;
volatile unsigned int g_Hour = 18;
volatile unsigned int g_Day = 1;
volatile unsigned int g_Mouth = 1;
volatile unsigned int g_Year = 1971;
volatile unsigned int g_CountChar = 0;

/* variable for enable mode on/off and enable display date/time */
volatile unsigned int g_Mode = 0;
volatile unsigned int g_EnaDisplay7Seg = 0;

/*variable for ringbuffer to recieve data from interrupt UART1 */
unsigned char Interrupt_Buffer[BUFFER_SIZE_MAX];
unsigned char Polling_Buffer[BUFFER_SIZE_MAX];
volatile unsigned char index_In = 0;
volatile unsigned char index_Out = 0;
volatile unsigned char index_cmdBuff = 0;
volatile unsigned char Rx_Byte_read;
volatile unsigned int sum;

/*************************************************************************************
* @description Converts a string (enclosed within '[' and ']') to an unsigned integer.
*              The function returns 0 if the string contains non-numeric characters.
* @param myString  The string to be converted to an unsigned integer.
* @return          The converted unsigned integer or 0 if an error occurred.
***************************************************************************************/
unsigned int toUint(const unsigned char* myString)
{
	unsigned int result = 0;
	
	g_CountChar = 0;
	while(myString[g_CountChar] != ']') 
			g_CountChar++;
	/* Convert string from Polling_Buffer to number */
	for(unsigned int i = 1; i < g_CountChar; i++)
	{
		if(myString[i] < '0' || myString[i] > '9')
			return -1;
		else
			result = result * 10 + (myString[i] - '0');
	}	

	return result;
}

/*************************************************************************************
* @description Converts an unsigned integer representing time in HHMMSS format to
*              hours, minutes, and seconds. Checks for validity of the time format.
* @param time  The unsigned integer to be converted (in HHMMSS format).
* @return      1 if the time format is valid, 0 otherwise.
***************************************************************************************/
unsigned int Convert_To_Time_Format(unsigned int time)
{
	/* Separate hours, minutes, seconds */
	unsigned int hours_temp = time / 10000;
	unsigned int minutes_temp = time / 100 % 100;
	unsigned int second_temp = time / 1 % 100;
	/* Check hours, minutes, seconds is valid */
	if (hours_temp > 23 || minutes_temp > 59 || second_temp > 59)
			return 0;    
	/* update hours, minutes, seconds  */
	g_Hour = hours_temp;
	g_Minute = minutes_temp;
	g_Second = second_temp;
    
  return 1;
}

/*************************************************************************************
* @description Converts an unsigned integer representing a date in DDMMYYYY format to 
*              days, months, and years. Checks for validity of the date format and 
*              leap year conditions.
* @param date  The unsigned integer to be converted (in DDMMYYYY format).
* @return      1 if the date format is valid, 0 otherwise.
***************************************************************************************/
unsigned int Convert_To_Date_Format(unsigned int date)
{
	/* Separate hours, minutes, seconds */
	unsigned int days_temp = date / 1000000;
	unsigned int mouths_temp = date / 10000 % 100;
	unsigned int years_temp = date % 10000;
	/* Check hours, minutes, seconds is valid */
	if(mouths_temp > 12 || mouths_temp < 1 || days_temp > getDaysInMonth(mouths_temp, years_temp) || days_temp < 1)
			return 0;
	/* update hours, minutes, seconds  */
	g_Day = days_temp;
	g_Mouth = mouths_temp;
	g_Year = years_temp;

	return 1;
}

/*************************************************************************************
* @description Updates the global time variables (seconds, minutes, hours, days, etc.) 
*              based on the number of elapsed seconds. Automatically increments the date 
*              when the day ends.
* @param       None.
***************************************************************************************/
void dateTime()
{
	if(g_CountSecond >= 10)
	{
		/*If the count is overcounted, the overcount value will be left for the next counting cycle*/
		g_CountSecond = g_CountSecond - 10; 
		g_Second++;
		if(g_Second == 60)
		{
			g_Minute++;
			g_Second = 0;
			if(g_Minute == 60)
			{
				g_Hour++;
				g_Minute = 0;
				if(g_Hour == 24)
				{
					g_Day++;
					g_Hour = 0;
					if(g_Day > getDaysInMonth(g_Mouth, g_Year))
					{
						g_Mouth++;
						g_Day = 1;
						if(g_Mouth == 13)
						{
							g_Year++;
							g_Mouth = 1;
						}
					}
				}
			}
		}
	}
}

/*************************************************************************************
* @description Processes the received data in the polling buffer, converts it into either 
*              time or date format, and updates the global variables. If the format is 
*              invalid, an error message is sent via UART.
* @param Buffer  The buffer containing the received string data to process.
***************************************************************************************/
void Process_Polling_Buffer(unsigned char *Buffer) 
{
	/*Convert string to Number*/
	sum = toUint(Buffer);
	/*Check format is time*/
	if(sum < 250000 && g_CountChar == 7)
		/*Check time is unvalid*/
		if(!Convert_To_Time_Format(sum))
		{
				LPUART1_Transmit_String((char *)"Invalid time format\n");
		}
		else
		{
				LPUART1_Transmit_String((char *)"Time updated successfully\n");
		}
	/*Check format is day*/
	else if(sum > 250000 && g_CountChar == 9)
		/*Check date is unvalid*/
		if(!Convert_To_Date_Format(sum))
		{
				LPUART1_Transmit_String((char *)"Invalid date format\n");
		}
		else
		{
				LPUART1_Transmit_String((char *)"Date updated successfully\n");
		}
	/* Check convert to Uint is unvalid */
	else if(sum == -1)
		LPUART1_Transmit_String((char *)"Invalid input format\n");
	/* Input string is unvalid */
	else
	{
		LPUART1_Transmit_String((char *)"Invalid input format\n");
	}
}

/*************************************************************************************
* @description Checks the receive buffer for incoming UART data. Processes the data when 
*              a complete command is received (enclosed in '[' and ']') and stores it in 
*              the polling buffer for further processing.
* @param       None.
***************************************************************************************/
void CheckRxBuffer(void)
{
	/* Check data come from uart */
	if ( index_Out != index_In ) 
	{
		Rx_Byte_read = Interrupt_Buffer[index_Out];

		if( Rx_Byte_read == '[' ) 
		{
			index_cmdBuff = 0;     /* Reset index cmd Buffer when data is '[' */
		}
			
		Polling_Buffer[index_cmdBuff] = Rx_Byte_read;
		
		if (index_cmdBuff < BUFFER_SIZE_MAX - 1)
		{
			index_cmdBuff++;        /* Increment index cmd Buffer */
		}
			
		if (Rx_Byte_read == ']')
		{   /* Check character stop is ']' */
				Polling_Buffer[index_cmdBuff] = 0;

				if(Polling_Buffer[0] == '[')  /* Check character start is '[' */
				{
					Process_Polling_Buffer(Polling_Buffer);
				}
						
				Polling_Buffer[0] = 0; /* Reset character start */
		}
		index_Out++;
		/*Check Overflow buffer */
		if(index_Out >= BUFFER_SIZE_MAX) 
			index_Out = 0;
	}
}

/*************************************************************************************
* @description Interrupt handler for PORTC. Handles the button presses to toggle 
*              `g_Mode` and `g_EnaDisplay7Seg` based on interrupts from the PORTC pins.
*              Clears the interrupt flags for both pins.
* @param       None.
***************************************************************************************/
void PORTC_IRQHandler(void) 
{ 
	/* Clear interrupt flag */
	volatile unsigned int* PORTC_PCR12 = (volatile unsigned int *)(PORT_BASE[PORT_C] + PORT_PCR[12]);
	volatile unsigned int* PORTC_PCR13 = (volatile unsigned int *)(PORT_BASE[PORT_C] + PORT_PCR[13]);
	/*Check pin that occur external interrupt in pin 12*/
	if(((*PORTC_PCR12 & (1 << 24)) >> 24) == 1) 
	{
		*PORTC_PCR12 |= (1 << 24); /* Clear the interrupt flag */
		g_Mode++;
		if(g_Mode == 3)
			g_Mode = 0; 
	}
	/*Check pin that occur external interrupt in pin 13*/
  if(((*PORTC_PCR13 & (1 << 24)) >> 24) == 1)
	{
		*PORTC_PCR13 |= (1 << 24); /* Clear the interrupt flag */
		g_EnaDisplay7Seg++;
		if(g_EnaDisplay7Seg == 2)
			g_EnaDisplay7Seg = 0; 
	}
}

/*************************************************************************************
* @description Interrupt handler for LPIT0 channel 0. Increments the `g_CountSecond` 
*              variable when the interrupt occurs and clears the interrupt flag.
* @param       None.
***************************************************************************************/
void LPIT0_Ch0_IRQHandler(void)
{
	// Reset flag interrupt for channel 0
	LPIT_MSR |= (1 < 0);
	/* increase value of variable  */
	g_CountSecond++;
}

/*************************************************************************************
* @description Interrupt handler for LPUART1. Reads the received byte from the UART data 
*              register and stores it in the interrupt buffer for later processing.
* @param       None.
***************************************************************************************/
void LPUART1_RxTx_IRQHandler(void)
{
	/* Add recieved data to Interrupt_Buffer */
//	while(index_In < BUFFER_SIZE_MAX)
//	{
	Interrupt_Buffer[index_In] = (unsigned char)LPUART1->DATA;
	/* Increase index_In to compare with index_Out to know when the data come in */
	index_In++;
	if(BUFFER_SIZE_MAX <= index_In)
	{
			index_In = 0;
	}
//	}
}
/*! end line !*/
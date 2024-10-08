#include"ConfigGPIO.h"
#include"ADC.h"
#include"SysTick.h"
#include"SPI_MAX7219.h"
#include"LPIT.h"
#include"UART1.h"
#include"Mock_project.h"

int main(void)
{
	/*Enable systicks*/
	SysTick_Init();
	
	/* Enable NVIC for LPIT to count time*/
	LPIT_Init_Interrupt();
	
	/* Enable NVIC for PORTC */
	Enable_PORTC_Interrupt();
	
	/* Enable uart1 NVIC */
	Enable_LPUART1_Interrupt();
	
	/* LPIT interrupt every 100ms */
	LPIT_Init(12800);
	
	/* Enable clock for ports */
	Gpio_Init(PORT_B); 
	Gpio_Init(PORT_C);
	
	/* Config ADC */
	ADC_Init();
	
	/* Mux 011 for SPI protocol */
	Gpio_SetMux(PORT_B, 14, 3);  /* LPSPI1_SCK */
	Gpio_SetMux(PORT_B, 15, 3);  /* LPSPI1_SIN */
	Gpio_SetMux(PORT_B, 16, 3);  /* LPSPI1_SOUT */
	Gpio_SetMux(PORT_B, 17, 3);  /* LPSPI1_PCS3 */
	
	/* Mux 010 for LPUART1 */
	Gpio_SetMux(PORT_C, 6, 2); /* tx1 */
	Gpio_SetMux(PORT_C, 7, 2); /* rx1 */
	
	/* Config for 2 button */
	Gpio_SetMode(PORT_C, 12, INPUT, RISING_EDGE);
	Gpio_SetMode(PORT_C, 13, INPUT, RISING_EDGE);
	
	/* FIRCCLK_DIV2 */
	Clock_FIRC_Enable();
	
	/* Initialize LPSPI1 */
  LPSPI_Init();
    
  /* Initialize MAX7219 */
  MAX7219_Init();

	/* Initilize LPUART1 */
	LPUART1_Init();
	
	/* Enable interrupt lpuart1 */
	UART1_EnableInterrupts();
	
	/* Display user manual */
	LPUART1_Transmit_String((char *)"Hello world, please enter following format to change time [hhmmss] and [ddmmyyyy]\n");	
	LPUART1_Transmit_String((char *)"EX1: [235030] , hours is 23 - minutes is 50 - seconds is 30\n");	
	LPUART1_Transmit_String((char *)"EX2: [22012002] , days is 22 - mouths is 01 - years is 2002\n");
	 
	/* Declare variables to store value from ADC module */
	unsigned int Pot_Value_Temp = 0;
	unsigned int Pot_Value = 0;
	unsigned int Temperature = 0;
	while(1)
	{
		/* Get real time */
		dateTime();
		
		/* check data  */
		CheckRxBuffer();
		
		/* Check mode (time/date) */
		if(g_Mode == 0)
			MAX7219_Display_Time(g_Hour, g_Minute, g_Second);
		else if(g_Mode == 1)
			MAX7219_Display_Date(g_Day, g_Mouth, g_Year);
		else
			MAX7219_Display_Temperature(Temperature);
		
		/* Check mode (On/Off) */
		if(g_EnaDisplay7Seg == 0)
			SPI_SendData(MAX7219_SHUTDOWN, 0x01);
		else
			SPI_SendData(MAX7219_SHUTDOWN, 0x00);	
		
		/* Adjust brightness for module leds 7-segments  */
		Convert_ADC_Channel(12);
		Pot_Value = Read_ADC_Result() * 15 / 4095; /* resolution of ADC 12bit from 0 -> 4095, brightness 0 - 15*/
		if(Pot_Value != Pot_Value_Temp) /* when Pot_Value is a changes, the Pot_Value will be recaculated */
		{
			/* Set intensity (brightness level 0-15) */
			SPI_SendData(MAX7219_INTENSITY, Pot_Value);
			/* update the current value to Pot_Value_Temp */
			Pot_Value_Temp = Pot_Value; 
		}
		
		/************* Display temperature ****************/
		/*
		* Stepsize = (vref+  -  vref-) / (2^n -1))
						 =  (5000mV)/(2^12 - 1) 
						 =  1,221 mv
		* Sensor LM35:   10mV ---> 1 Celsius
		* S32k144: 			 1.221mV ---> 1 stepsize
		* ==> diference = 10/1.221 = 8.2
		*/
		for(int i = 0; i < 100; i++)
		{
			Convert_ADC_Channel(13);
		  Temperature += (Read_ADC_Result() / 8.2);
		}
		Temperature /= 100; /* Caculate average value for 100 times Read ADC */
	}
	return 0;
}

#include"SPI_MAX7219.h"

/* code 7 led segment (0 - 9) */
const unsigned char digitSegmentCodes[10] = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};
																						/* 0     1     2     3     4    5     6      7     8     9*/
const unsigned char greeting[8] = {0x37, 0x4f, 0x0e, 0x0e, 0x7e, 0x00, 0x00, 0x00};
																		/*H     E    L      L     O   */
/*************************************************************************************
* @description Initializes the LPSPI interface for SPI communication. Configures clock 
*              source, clock polarity, phase, prescaler, and other control parameters 
*              for SPI communication.
* @param       None.
***************************************************************************************/
void LPSPI_Init(void)
{
	/* Disable clock to config */
	PCC_LPSPI1 &= ~(1 << 30);
	/* Select source LPSPI */
  PCC_LPSPI1 |= (3 << 24); // Peripheral Clock Source Select 011 -> FIRCCLK_DIV2	
	/*  Enable clock */
	PCC_LPSPI1 |= (1 << 30);

	LPSPI->TCR = 0;
	/* Prescale 2**2 = 4 */
	LPSPI->TCR |= (2 << 27);
	
	/* Configure clock prescaler and SCK divider					*/
	/* SCKPCS=4: SCK to PCS delay = 4+1 = 5 (500 nsec) 		*/
	/* PCSSCK=4: PCS to SCK delay = 9+1 = 10 (1 usec) 		*/
	/* DBT=8: Delay between Transfers = 8+2 = 10 (1 usec) */
	LPSPI->CCR |= (4 << 24) | (4 << 16) | (8 << 8) | (10 << 0);  // SCKDIV= 10 + 2 = 12.  for 1MHz
	
	//Configures Clock Phase and Polarity
  LPSPI->TCR |= (0 << 30) | (0 << 31) | (3 << 24) | ((16 - 1) << 0); // CPOL=0, CPHA=0,PCS3, 16 bit
	
	/* TXWATER=2: Tx flags set when Tx FIFO <= 3 	*/
	LPSPI->FCR |= 1;

	/* Set as Master */
	LPSPI->CFGR1 |= (1 << 0);
	
	/* transmit continous */
	LPSPI->CFGR1 |= (1 << 3);
	
	/* DBGEN=1: module enabled in debug mode 
		 MEN=1: Module is enabled */
	LPSPI->CR |= (1 << 3) | (1 << 0);
}

/*************************************************************************************
* @description Transmits 16-bit data via the LPSPI interface. Waits for the Transmit Data 
*              Register Empty (TDRE) flag before sending the data.
* @param data  The 16-bit data to be transmitted.
***************************************************************************************/
void LPSPI_transmit_16bit(short data)
{
	// Wait for TDRE flag
	while (!(LPSPI->SR & (1 << 0)));
	
	// Transmit character
	LPSPI->TDR = data;
	
	// Clear TDRE flag
	LPSPI->SR |= (1 << 0);
}

/*************************************************************************************
* @description Sends a 16-bit data packet (combining address and data) to the MAX7219 
*              via SPI. The function waits until the Transmit Data Register Empty (TDRE) 
*              flag is set before sending the data.
* @param address  The register address on the MAX7219 to which the data will be sent.
* @param data     The data to be sent to the specified register.
***************************************************************************************/
void SPI_SendData(unsigned char address, unsigned char data)
{
	unsigned short packet = (address << 8) | data; // Combine address and data
	
	// Wait for TDRE flag (Transmit Data Register Empty)
	while (!(LPSPI->SR & (1 << 0)));
	
	// Transmit 16-bit data (address + data in one go)
	LPSPI->TDR = packet;
}

/*************************************************************************************
* @description Initializes the MAX7219 module for controlling 8 seven-segment LEDs. 
*              Configures decode mode, scan limit, intensity (brightness), and enables 
*              the display by exiting shutdown mode.
* @param       None.
***************************************************************************************/
void MAX7219_Init(void)
{
	// Set decode mode to no-decode (each bit controls individual segments)
	SPI_SendData(MAX7219_DECODE_MODE, 0x00);
	
	// Set scan limit to display all 8 digits (0-7)
	SPI_SendData(MAX7219_SCAN_LIMIT, 0x07);
	
	// Set intensity (brightness level 0-15)
	SPI_SendData(MAX7219_INTENSITY, 0x08);
	
	// Exit shutdown mode and enable display
	SPI_SendData(MAX7219_SHUTDOWN, 0x01);
}

/*************************************************************************************
* @description Displays a number (up to 8 digits) on the seven-segment LEDs using the 
*              MAX7219 module. The number is broken down into individual digits and 
*              each digit is sent to the corresponding segment.
* @param number  The number to be displayed (maximum 8 digits).
***************************************************************************************/
void MAX7219_Display_Number(unsigned int number)
{
	// Break number into digits and send to MAX7219
	for (int i = 0; i < 8; i++)
	{
		SPI_SendData(MAX7219_DIGIT0 + i, digitSegmentCodes[number % 10]);  // Send digit to display
		number /= 10;
	}
}

/*************************************************************************************
* @description Displays time on a MAX7219 module controlling 8 seven-segment LEDs.
*               The time is displayed in HH-MM-SS format with '-' as the separator.
* @param hours   The hours component of the time to display (0-23).
* @param minutes The minutes component of the time to display (0-59).
* @param seconds The seconds component of the time to display (0-59).
***************************************************************************************/
void MAX7219_Display_Time(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
	/* Display seconds */
	SPI_SendData(MAX7219_DIGIT0, digitSegmentCodes[seconds % 10]);
	SPI_SendData(MAX7219_DIGIT1, digitSegmentCodes[seconds / 10]);
	SPI_SendData(MAX7219_DIGIT2, 0x01); /* disple '-' to split */
	/* Display minutes*/
	SPI_SendData(MAX7219_DIGIT3, digitSegmentCodes[minutes % 10]);
	SPI_SendData(MAX7219_DIGIT4, digitSegmentCodes[minutes / 10]);
	SPI_SendData(MAX7219_DIGIT5, 0x01); /* disple '-' to split */
	/* Display hours */
	SPI_SendData(MAX7219_DIGIT6, digitSegmentCodes[hours % 10]);
	SPI_SendData(MAX7219_DIGIT7, digitSegmentCodes[hours / 10]);
}

/*************************************************************************************
* @description Displays a date on a MAX7219 module controlling 8 seven-segment LEDs.
*               The date is displayed in DDMMYYYY format.
* @param days   The day component of the date to display (may be 1-31).
* @param mouths The month component of the date to display (1-12).
* @param years  The year component of the date to display (e.g., 2024).
***************************************************************************************/
void MAX7219_Display_Date(unsigned int days, unsigned int mouths, unsigned int years)
{
	/* Display days */
	SPI_SendData(MAX7219_DIGIT7, digitSegmentCodes[days / 10]);
	SPI_SendData(MAX7219_DIGIT6, digitSegmentCodes[days % 10]);
	/* Display mouth */
	SPI_SendData(MAX7219_DIGIT5, digitSegmentCodes[mouths / 10]);
	SPI_SendData(MAX7219_DIGIT4, digitSegmentCodes[mouths % 10]);
	/* Display years */
	SPI_SendData(MAX7219_DIGIT3, digitSegmentCodes[years / 1000 % 10]);
	SPI_SendData(MAX7219_DIGIT2, digitSegmentCodes[years / 100 % 10]);
	SPI_SendData(MAX7219_DIGIT1, digitSegmentCodes[years / 10 % 10]);
	SPI_SendData(MAX7219_DIGIT0, digitSegmentCodes[years / 1 % 10]);
}

void MAX7219_Display_Temperature(unsigned int temp)
{
	/* Display temperature */
	SPI_SendData(MAX7219_DIGIT7, 0x01);
	SPI_SendData(MAX7219_DIGIT6, 0x01);
	SPI_SendData(MAX7219_DIGIT5, digitSegmentCodes[temp / 10]);
	SPI_SendData(MAX7219_DIGIT4, digitSegmentCodes[temp % 10]);
	SPI_SendData(MAX7219_DIGIT3, 0x63);
	SPI_SendData(MAX7219_DIGIT2, 0x4e);
	SPI_SendData(MAX7219_DIGIT1, 0x01);
	SPI_SendData(MAX7219_DIGIT0, 0x01);
	
}







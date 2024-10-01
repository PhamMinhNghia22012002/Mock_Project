#include"ConfigGPIO.h"
#include"UART1.h"

/*************************************************************************************
* @description Initializes LPUART1 for communication with 9600 baud rate, 1 stop bit, 
*              8-bit data format, and no parity. Configures clock source and baud rate.
* @param       None.
***************************************************************************************/
void LPUART1_init(void)  
{
	setClearBit(PCC_UART1_CLOCK, 30, RESET_BIT); /*  Ensure clk disabled for config */
	setClearBits(PCC_UART1_CLOCK, 24, 0x07, 0x03); /* Clock Src = 011 (fircDIV2_CLK)*/ 
	setClearBit(PCC_UART1_CLOCK, 30, SET_BIT); /* Enable clock for LPUART1 regs */
	
	LPUART1->BAUD = 0u;
	/* OSR=19 (0x13): Over sampling ratio = 19+1=20.*/
	/* SBR=250 (0xFA): baud divisor = 48M/9600/20 = 250. */
	LPUART1->BAUD = (19u << 24) | (250u << 0);
	/* SBNS=0: One stop bit */
	/* Enable transmitter & receiver, no parity, 8 bit char: */
	LPUART1->CTRL |= 0 << 4;
	/* M7,M,R8T9,R9T8=0: 8-bit data characters*/
	LPUART1->CTRL |= 3u << 18;
	/* PE,PT=0: No hw parity generation or checking */
}

/*************************************************************************************
* @description Transmits a single character via LPUART1. The function waits for the 
*              transmit buffer to be empty before sending the character.
* @param send  The character to transmit.
***************************************************************************************/
void LPUART1_transmit_char(char send) 
{    
	/* Wait for transmit buffer to be empty */
	while((LPUART1->STAT & (1u << 23)) >> 23 == 0); /* Transmit Data Register Empty Flag */
	
	/* Send data */
	LPUART1->DATA = send;              
}
	
/*************************************************************************************
* @description Transmits a null-terminated string via LPUART1. The function sends each 
*              character one by one using the `LPUART1_transmit_char` function.
* @param data_string  Pointer to the string to transmit.
***************************************************************************************/
void LPUART1_transmit_string(char *data_string)  
{  
	unsigned int i=0;
	while(data_string[i] != '\0')  
	{           
		/* Send chars one at a time */
		LPUART1_transmit_char(data_string[i]);
		i++;
	}
}
	
/*************************************************************************************
* @description Receives a single character from LPUART1. The function waits for the 
*              receive buffer to be full before reading the character.
* @return      The received character.
***************************************************************************************/
char LPUART1_receive_char(void) 
{    
	char receive;
  while((LPUART1->STAT & (1u << 21)) >> 21 == 0);
	/* Wait for received buffer to be full */
	
	/* Read received data*/
	receive= LPUART1->DATA;            
	return receive;
}

/*************************************************************************************
* @description Receives a string of up to 8 characters (including null terminator) via 
*              LPUART1. The function stops receiving when a newline ('\n') or ']' character
*              is received, or the buffer is full (8 characters max).
* @param receive  Pointer to a buffer where the received string will be stored.
***************************************************************************************/
void LPUART1_receive_string(char* receive) 
{     
    unsigned int i = 0;
		for(int i = 0; i < 9; i++)
		{
			receive[i] = '\0';
		}
    while(1)
    {
        while((LPUART1->STAT & (1u << 21)) >> 21 == 0); // Wait for received buffer to be full
        
        char received_char = LPUART1->DATA; // Read received data
        if(received_char == '\n' || i >= 9 - 1)
        {
            receive[i] = '\0';  // Terminate the string
            break;
        }
        else if(received_char == ']')
				{
					receive[i] = received_char; /* Store the received character */
					break;
				}
				else
				{
					receive[i] = received_char;
				}
        i++;
    }
}

/*************************************************************************************
* @description Receives a single character from LPUART1 and echoes it back by transmitting 
*              the same character. The function also sends a newline character ('\n') after 
*              the echoed character.
* @param       None.
***************************************************************************************/
void LPUART1_receive_and_echo_char(void)  
{  
	char send = LPUART1_receive_char();        /* Receive Char */
	LPUART1_transmit_char(send);               /* Transmit same char back to the sender */
	LPUART1_transmit_char('\n');               /* New line */
}

/*************************************************************************************
* @description Receives a string via LPUART1 and echoes it back by transmitting the entire 
*              received string. A newline character is transmitted after the echoed string.
* @param       None.
***************************************************************************************/
void LPUART1_receive_and_echo_string(void)  
{  
	char received_string[9]; 
	LPUART1_receive_string(received_string);        /* Receive Char */
	LPUART1_transmit_string(received_string);       /* Transmit same char back to the sender */
	LPUART1_transmit_char('\n'); 
}

/*************************************************************************************
* @description Enables the receiver interrupt (RIE) for LPUART1, allowing interrupts 
*              to be generated when data is received.
* @param       None.
***************************************************************************************/
void UART1_EnableInterrupts(void)
{
    /*Receiver Interrupt Enable (RIE) and Transmit Interrupt Enable (TIE) */
		LPUART1->CTRL |= (1u << 21);
}


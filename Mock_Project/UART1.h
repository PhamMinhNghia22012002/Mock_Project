#ifndef _UART1_
#define _UART1_

// define register
#define SCG_BASE           0x40064000u
#define SCG_FIRCDIV2			(volatile unsigned int *)(SCG_BASE + 0x304u)

#define PCC_BASE            0x40065000u
#define PCC_UART1_CLOCK     (volatile unsigned int *)(0x40065000u + 0x1ACu)
void LPUART1_RxTx_IRQHandler(void);
typedef struct 
{
	volatile unsigned int	VERID	;
	volatile unsigned int	PARAM 	;
	volatile unsigned int	GLOBAL 	;
	volatile unsigned int	PINCFG 	;
	volatile unsigned int	BAUD 	;
	volatile unsigned int	STAT 	;
	volatile unsigned int	CTRL 	;
	volatile unsigned int	DATA 	;
	volatile unsigned int	MATCH 	;
	volatile unsigned int	MODIR 	;
	volatile unsigned int	FIFO 	;
	volatile unsigned int	WATER 	;
} LPUART_Type;

#define LPUART1_base_address (0x4006B000u)
#define LPUART1 ((LPUART_Type*) LPUART1_base_address)

// function prototype
void LPUART1_init(void);
void LPUART1_transmit_char(char send) ;
void LPUART1_transmit_string(char *data_string); 
char LPUART1_receive_char(void) ;
void LPUART1_receive_string(char* receive); 
void LPUART1_receive_and_echo_char(void)  ;
void LPUART1_receive_and_echo_string(void)  ;
void UART1_EnableInterrupts(void);

#endif

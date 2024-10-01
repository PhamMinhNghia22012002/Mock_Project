#ifndef __Mock_project__
#define __Mock_project__
#include"ConfigGPIO.h"
#include"UART1.h"
#include"LPIT.h"

/* define register */
#define SCG_BASE           					0x40064000u
#define SCG_FIRCDIV2								(volatile unsigned int *)(SCG_BASE + 0x304u)
#define Clock_FIRC_Enable()					*(SCG_FIRCDIV2 )|= (1 << 8);
#define Enable_LPUART1_Interrupt() 	*((volatile unsigned int *)(0xE000E100 + 4 * (33/32))) |= 1 << (33%32);
#define Enable_PORTC_Interrupt()		setClearBit((unsigned int *)(0xE000E100u + 4*1), 29U, SET_BIT); 
/* max size of ring buffer*/
#define BUFFER_SIZE_MAX   (11u)

/* variable for count the value of datetime */
extern volatile unsigned int g_CountSecond;
extern volatile unsigned int g_Second;
extern volatile unsigned int g_Minute;
extern volatile unsigned int g_Hour;
extern volatile unsigned int g_Day;
extern volatile unsigned int g_Mouth;
extern volatile unsigned int g_Year;
extern volatile unsigned int g_CountChar;

/* variable for enable mode on/off and enable display date/time */
extern volatile unsigned int g_Mode;
extern volatile unsigned int g_EnaDisplay7Seg ;

/*variable for ringbuffer to recieve data from interrupt UAR1 */
extern unsigned char Interrupt_Buffer[BUFFER_SIZE_MAX];
extern unsigned char Polling_Buffer[BUFFER_SIZE_MAX];
extern volatile unsigned char index_In;
extern volatile unsigned char index_Out;
extern volatile unsigned char index_cmdBuff;
extern volatile unsigned char Rx_Byte_read;
extern volatile unsigned int sum;

/* function prototype */
unsigned int toUint(const unsigned char* myString);
unsigned int Convert_To_Time_Format(unsigned int time);
unsigned int Convert_To_Date_Format(unsigned int date);
void dateTime();
void Process_Polling_Buffer(unsigned char *Buffer); 
void CheckRxBuffer(void);
void PORTC_IRQHandler(void) ;
void LPIT0_Ch0_IRQHandler(void);
void LPUART1_RxTx_IRQHandler(void);
void MAX7219_Display_Temperature(unsigned int temp);

#endif

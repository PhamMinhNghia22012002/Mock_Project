#ifndef __SPI_MAX7219__
#define __SPI_MAX7219__

// MAX7219 Registers
#define MAX7219_DIGIT0       0x01
#define MAX7219_DIGIT1       0x02
#define MAX7219_DIGIT2       0x03
#define MAX7219_DIGIT3       0x04
#define MAX7219_DIGIT4       0x05
#define MAX7219_DIGIT5       0x06
#define MAX7219_DIGIT6       0x07
#define MAX7219_DIGIT7       0x08
#define MAX7219_DECODE_MODE  0x09
#define MAX7219_INTENSITY    0x0A
#define MAX7219_SCAN_LIMIT   0x0B
#define MAX7219_SHUTDOWN     0x0C

/* code 7 led segment (0 - 9) */
extern const unsigned char digitSegmentCodes[10];
extern const unsigned char greeting[8];

// Define LPSPI memory structure
typedef struct
{
    volatile unsigned int VERID;
    volatile unsigned int PARAM;
    unsigned int rev1;
    unsigned int rev2;
    volatile unsigned int CR;
    volatile unsigned int SR;
    volatile unsigned int IER;
    volatile unsigned int DER;
    volatile unsigned int CFGR0;
    volatile unsigned int CFGR1;
    unsigned int rev3;
    unsigned int rev4;
    volatile unsigned int DMR0;
    volatile unsigned int DMR1;
    unsigned int rev5;
    unsigned int rev6;
    volatile unsigned int CCR;
    unsigned int rev7;
    unsigned int rev8;
    unsigned int rev9;
    unsigned int rev10;
    unsigned int rev11;
    volatile unsigned int FCR;
    volatile unsigned int FSR;
    volatile unsigned int TCR;
    volatile unsigned int TDR;
    unsigned int rev12;
    unsigned int rev13;
    volatile unsigned int RSR;
    volatile unsigned int RDR;
} LPSPI_Type;

#define LPSPI0_base_address (0x4002C000u)
#define LPSPI1_base_address (0x4002D000u)
#define LPSPI2_base_address (0x4002E000u)
#define LPSPI	((LPSPI_Type*)LPSPI1_base_address)

#define PCC_BASE            0x40065000u
#define PCC_LPSPI1      (*(volatile unsigned int *)(PCC_BASE + 0xb4u)) // Offset for LPSPI1 PCC
	
/* fuction prototype */
void LPSPI_Init();
void LPSPI_transmit_16bit(short data);
void SPI_SendData(unsigned char address, unsigned char data);
void MAX7219_Init(void);
void MAX7219_Display_Number(unsigned int number);

void MAX7219_Display_Time(unsigned int hours, unsigned int minutes, unsigned int seconds);
void MAX7219_Display_Date(unsigned int days, unsigned int mouths, unsigned int years);

#endif

#ifndef _ConfigGPIO_
#define _ConfigGPIO_

#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3
#define PORT_E 4

#define PCC_BASE 0x40065000u

#define PDDR 0x14u
#define PDOR 0x00u
#define PDIR 0x10u
#define PTOR 0x0Cu

#define SET_BIT   1
#define RESET_BIT   0

#define INPUT   0
#define OUTPUT   1

//modes of interrupt
#define NO_IRQ 0xFFu
#define LOGIC_0   0x08
#define RISING_EDGE   0x09u
#define FALLING_EDGE   0x0Au
#define EITHER_EDGE   0x0Bu
#define LOGIC_1   0x0Cu
#define DISABLE_INTERRUPT   0x00u

extern const unsigned int PCC_PORT[5];
extern const unsigned int PORT_BASE[5];
extern const unsigned int GPIO_BASE[5];
extern const unsigned int PORT_PCR[32];

// function prototype
void setClearBit(volatile unsigned int *address, unsigned char position, unsigned char action);
void setClearBits(volatile unsigned int *adddress, unsigned char position, unsigned char mask, unsigned char value);
void Gpio_Init(unsigned int port);
void Gpio_SetMode(unsigned int port, unsigned char pin, unsigned char direct, unsigned char IRQ);
void Gpio_WriteChannel(unsigned int port, unsigned char pin, unsigned char level);
void Gpio_ToggleChannel(unsigned int port, unsigned char pin);
unsigned int Gpio_ReadChannel(unsigned int port, unsigned char pin); 
void ClearFlagExternalInterruptPortC(unsigned char pin);
void Gpio_SetMux(unsigned int port, unsigned char pin, unsigned int mux);

#endif
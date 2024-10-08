#include"ConfigGPIO.h"

/* declare pcc address and base address */
const unsigned int PCC_PORT[5] = {0x124u, 0x128u, 0x12Cu, 0x130u, 0x12Cu};
const unsigned int PORT_BASE[5] = {0x40049000u, 0x4004A000u, 0x4004B000u, 0x4004C000u, 0x4004D000u};
const unsigned int GPIO_BASE[5] = {0x400FF000u, 0x400FF040u, 0x400FF080u, 0x400FF0C0u, 0x400FF100u};
const unsigned int PORT_PCR[32] = {0x00u, 0x04u, 0x08u, 0x0Cu, 0x10u, 0x14u, 0x18u, 0x1Cu, 0x20u, 0x24u, 0x28u, 0x2Cu, 0x30u, 0x34u, 0x38u, 0x3Cu, 
																	0x40u, 0x44u, 0x48u, 0x4Cu, 0x50u, 0x54u, 0x58u, 0x5Cu, 0x60u, 0x64u, 0x68u, 0x6Cu, 0x70u, 0x74u, 0x78u, 0x7Cu};

/*******************************************************************************
 * @brief Set or clear a specific bit at a given address.
 * 
 * @param address: Pointer to the register address where the bit will be modified.
 * @param position: Bit position to be set or cleared (0-31).
 * @param action: Action to perform: SET_BIT (1) or RESET_BIT (0).
********************************************************************************/
void setClearBit(volatile unsigned int *address, unsigned char position, unsigned char action) 
{
    if (action == SET_BIT) 
    {
        *address |= (1 << position);  // Thiet lap bit
    } 
    else if (action == RESET_BIT) 
    {
        *address &= ~(1 << position); // X?a bit
    }
}

/*********************************************************************************
 * @brief Set or clear multiple bits at a specific location in a register.
 * 
 * @param address: Address of the register to set/clear bits.
 * @param position: Starting position of the bits to be set/cleared.
 * @param mask: Bit mask identifies which bits to change.
 * @param value: The new value of the bits determined by the mask.
**********************************************************************************/
void setClearBits(volatile unsigned int *adddress, unsigned char position, unsigned char mask, unsigned char value) 
{
  volatile unsigned int* l_address;
	volatile unsigned int  l_value;
	l_address = adddress;
	l_value = *l_address;
	
  l_value &= (unsigned int)(~(mask << position));
	l_value |= value << position;
	*l_address = l_value;
}

/**********************************************************************************
 * @brief Initialize the clock for a specific GPIO port.
 * 
 * @param port: GPIO port to be initialized (PORT_A, PORT_B, PORT_C, PORT_D, PORT_E).
***********************************************************************************/
void Gpio_Init(unsigned int port)
{
	volatile unsigned int* PCC = (volatile unsigned int *)(PCC_BASE + PCC_PORT[port]); // pulse clock
	/* ENABLE CLOCK */
	setClearBit(PCC, 30, SET_BIT);
}

void Gpio_SetMux(unsigned int port, unsigned char pin, unsigned int mux)
{
	// gpio configuration
	volatile unsigned int* PORT_PCRn = (volatile unsigned int *)(PORT_BASE[port] + PORT_PCR[pin]); 
	// set mux
	setClearBits(PORT_PCRn, 8, 0x07, mux); 
}

/***********************************************************************************
 * @brief Configure a specific pin on a GPIO port.
 * 
 * @param port: GPIO port number (PORT_A, PORT_B, PORT_C, PORT_D, PORT_E).
 * @param pin: Pin number on the specified port (0-31).
 * @param direct: Direction of the pin: INPUT (0) or OUTPUT (1).
 * @param IRQ(Interrupt configuration mode): NO_IRQ, RISING_EDGE, FALLING_EDGE, etc.
*************************************************************************************/
void Gpio_SetMode(unsigned int port, unsigned char pin, unsigned char direct, unsigned char IRQ)
{
	// gpio configuration
	volatile unsigned int* PORT_PCRn = (volatile unsigned int *)(PORT_BASE[port] + PORT_PCR[pin]); 
	// mux 001
	setClearBits(PORT_PCRn, 8, 0x07, 0x01); 
	//config direction
	volatile unsigned int* GPIOn_PDDR = (volatile unsigned int *)(GPIO_BASE[port] + PDDR);
	setClearBit(GPIOn_PDDR, pin, direct);
	// Configure interrupt if not OUTPUT and IRQ is not NO_IRQ
  if (direct != OUTPUT && IRQ != NO_IRQ)
  {
		setClearBits(PORT_PCRn, 16, 0x0f, IRQ); 
  }
}

/*************************************************************************************
 * @brief Set the output level of a specific GPIO pin.
 * 
 * @param port GPIO port number (PORT_A, PORT_B, PORT_C, PORT_D, PORT_E).
 * @param pin Pin number on the specified port (0-31).
 * @param level Output level to be set: SET_BIT (1) or RESET_BIT (0).
*************************************************************************************/
void Gpio_WriteChannel(unsigned int port, unsigned char pin, unsigned char level)
{
	volatile unsigned int* GPIOn_PDOR = (volatile unsigned int *)(GPIO_BASE[port] + PDOR);
	// write level to specific pin
	setClearBit(GPIOn_PDOR, pin, level);
}

/************************************************************************************
 * @brief Toggle the output level of a specific GPIO pin.
 * 
 * @param port GPIO port number (PORT_A, PORT_B, PORT_C, PORT_D, PORT_E).
 * @param pin Pin number on the specified port (0-31).
************************************************************************************/
void Gpio_ToggleChannel(unsigned int port, unsigned char pin)
{
	volatile unsigned int* GPIOn_PTOR = (volatile unsigned int *)(GPIO_BASE[port] + PTOR);
	/* Toggle the pin level */
	setClearBit(GPIOn_PTOR, pin, SET_BIT);
}

/**************************************************************************************
 * @brief Read the input level of a specific GPIO pin.
 * 
 * @param port: GPIO port number (PORT_A, PORT_B, PORT_C, PORT_D, PORT_E).
 * @param pin: Pin number on the specified port (0-31).

 * @return unsigned int The current level of the pin: 1 if high, 0 if low.
***************************************************************************************/
unsigned int Gpio_ReadChannel(unsigned int port, unsigned char pin)
{
	volatile unsigned int* GPIOn_PDIR = (volatile unsigned int *)(GPIO_BASE[port] + PDIR);
	//read state of pin
	unsigned int state = (*GPIOn_PDIR & (1 << pin)) >> pin;
	
	return state;
}

/**************************************************************************************
 * @brief Clear the interrupt flag for a specific pin on PORTC.
 * 
 * @param pin: Pin number on PORTC (0-31) whose interrupt flag needs to be cleared.
***************************************************************************************/
void ClearFlagExternalInterruptPortC(unsigned char pin)
{
	volatile unsigned int* PORTn_PCRn = (volatile unsigned int *)(PORT_BASE[PORT_C] + PORT_PCR[pin]);
  *PORTn_PCRn |= (1 << 24); /* Clear the interrupt flag */
}
/*! end line !*/
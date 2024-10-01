#include"ADC.h"

/*************************************************************************************
* @description Initializes the ADC0 module for software-triggered conversion. The 
*              configuration includes selecting the clock source, setting the prescaler, 
*              and configuring the ADC for 12-bit resolution with a sample time of 13 ADC clocks.
* @param	None
***************************************************************************************/
void ADC_Init()
{
	PCC_ADC0_INDEX &= ~(1 << 30); 		/* Disable clock to change PCS */
	PCC_ADC0_INDEX |= (3 << 24);			/* PCS=011: Select FIRCDIV2 		*/
	PCC_ADC0_INDEX |= (1 << 30);			/* Enable bus clock in ADC 	*/
	
	/*================= ADC initialization ==================*/
	ADC0_SC1 &= ~(1 << 6); /* AIEN=0: Interrupts are disabled 			*/
	//ADC0_SC1 |= 0x3f;			/* ADCH=1F: Module is disabled for conversions	*/
	
	ADC0_CFG1 |= (0 << 5)| /* ADIV=0: Prescaler=1 					*/
								(1 << 2)| /* MODE=1: 12-bit conversion 				*/
								(0 << 0); /* Only ALTCLK1 is available */
	ADC0_CFG2 |= (12 << 0);  /* SMPLTS=12(default): sample time is 13 ADC clks 	*/
	
	/* ADTRG=0: SW trigger, DMAEN=0: DMA disabled, REFSEL=0: Voltage reference pins= VREFH, VREEFL, ACFE,ACFGT,ACREN=0: Compare func disabled	*/
	ADC0_SC2 = 0x00000000;
	
  /* AVGE,AVGS=0: HW average function disabled, ADCO=0: One conversion performed, CAL=0: Do not start calibration sequence*/
	ADC0_SC3 = 0x00000000;
}

/*************************************************************************************
* @description Initializes the ADC0 module for hardware-triggered conversion. It 
*              configures the ADC with a 12-bit resolution and a sample time of 13 ADC clocks. 
*              The hardware trigger is enabled.
* @param None
***************************************************************************************/
void ADC_HWtrigger_Init()
{
	PCC_ADC0_INDEX &= ~(1 << 30); 		/* Disable clock to change PCS */
	PCC_ADC0_INDEX |= (3 << 24);			/* PCS=011: Select FIRCDIV2 		*/
	PCC_ADC0_INDEX |= (1 << 30);			/* Enable bus clock in ADC 	*/
	
	/*================= ADC initialization ==================*/
	ADC0_SC1 &= ~(1 << 6); /* AIEN=0: Interrupts are disabled 			*/
	//ADC0_SC1 |= 0x3f;			/* ADCH=1F: Module is disabled for conversions	*/
	
	ADC0_CFG1 |= (0 << 5)| /* ADIV=0: Prescaler=1 					*/
								(1 << 2)| /* MODE=1: 12-bit conversion 				*/
								(0 << 0); /* Only ALTCLK1 is available */
	ADC0_CFG2 |= (12 << 0);  /* SMPLTS=12(default): sample time is 13 ADC clks 	*/
	
	/* ADTRG=1: SW trigger enb, DMAEN=0: DMA disabled, REFSEL=0: Voltage reference pins= VREFH, VREEFL, ACFE,ACFGT,ACREN=0: Compare func disabled	*/
	ADC0_SC2 = (1 << 6);
	
  /* AVGE,AVGS=0: HW average function disabled, ADCO=0: One conversion performed, CAL=0: Do not start calibration sequence*/
	ADC0_SC3 = 0x00000000;
}

/*************************************************************************************
* @description Selects the ADC channel for conversion by clearing the prior channel 
*              selection and starting a conversion on the specified channel.
* @param  The ADC channel number to be selected for conversion.
***************************************************************************************/
void Convert_ADC_Channel(unsigned int adcChannel)
{
	ADC0_SC1 &= ~( 0x3f ); /* Clear prior ADCH bits 	*/
	ADC0_SC1 |= (adcChannel << 0); /* Start coversion with channel */
}

/*************************************************************************************
* @description Reads the result of the ADC conversion. The function waits for the 
*              conversion to complete and then returns the conversion result.
* @return The ADC conversion result from the R0 register.
***************************************************************************************/
unsigned int Read_ADC_Result(void)
{
	unsigned int result = 0;
	while(((ADC0_SC1 & (1 << 7)) >> 7) == 0); /* Wait for completion convesion*/
	result = ADC0_R0; /* For SW trigger mode, R0 is used 	*/
	
	return result;
}


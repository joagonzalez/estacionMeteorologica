// Trabajo practico Final
// Materia: Electronica Digital II - 2019 (ECyT - UNSAM)
//
// Docentes:
//	- Sagreras Miguel
//	- Alvarez Nicolas
// Alumnos:
// 	- Gonzalez Joaquin - joagonzalez@gmail.com
// 	- Pedraza Sebastian - sebastianpedraza2002@yahoo.com.ar

/************************************************************************************
	ADC	(Conversor Analogico-Digital)																	    
 ************************************************************************************/
#define ADC0_BASE		0x400E3000
#define ADC0			((ADC_T *) 	ADC0_BASE)
 
// Registros para el ADC
typedef struct {	  // ADCn Structure */
	unsigned int CR;  // A/D Control Register. The AD0CR register must be written to select the operating mode before A/D conversion can occur. */
	unsigned int GDR; // A/D Global Data Register. Contains the result of the most recent A/D conversion. */
	unsigned int RESERVED0;
	unsigned int INTEN;	// A/D Interrupt Enable Register. This register contains enable bits that allow the DONE flag of each A/D channel to be included or excluded from contributing to the generation of an A/D interrupt. */
	unsigned int DR[8];	// A/D Channel Data Register. This register contains the result of the most recent conversion completed on channel n. */
	unsigned int STAT;	// A/D Status Register. This register contains DONE and OVERRUN flags for all of the A/D channels, as well as the A/D interrupt flag. */
} ADC_T;


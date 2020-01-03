#include "board.h"
#include <math.h>
#include "estructuras.h"

#define ADC_CHANNEL ADC_CH1
#define ADC_ID		ADC0		// #define ADC0_BASE		0x400E3000
								// #define ADC0 ((ADC_T *) 	ADC0_BASE)

static volatile uint8_t ADC_Interrupt_Done_Flag;

static char aux[256];	// para debugging (usada por printf)
int i = 0;

/* Impresion del valor adquirido y generacion de retardo */
static void App_print_ADC_value(uint16_t data){
		sprintf_mio(aux, "%d\r\n", data);
		DEBUGSTR(aux);
		for (i=0; i<10000000; i++);
}

unsigned int tmp;

/* Funcion manejadora de la interrupcion del ADC */
void ADC0_IRQHandler(void)
{
	NVIC_DesIRQ(ADC0_IRQn);
	//Chip_ADC_Int_SetChannelCmd(LPC_ADC_ID, ADC_CHANNEL, DISABLE);
	//ADC0->INTEN &= (~1UL << ADC_CHANNEL);
	
	/* Lectura del valor de adquirido */
	// DR = Registro en el que se almacena el valor adquirido
	tmp = ADC0->DR[ADC_CHANNEL];
	tmp = (tmp >> 6) & 0x03FF;
	
	ADC_Interrupt_Done_Flag = 1;
	
	App_print_ADC_value(tmp);
	NVIC_EnaIRQ(ADC0_IRQn);
	
	//Chip_ADC_Int_SetChannelCmd(LPC_ADC_ID, ADC_CHANNEL, ENABLE);
	//ADC0->INTEN |= (1UL << ADC_CHANNEL);

}


static void ADC_Config(void) {

	ADC0->INTEN = 0; 				// Deshabilitacion de las interrupcion generada
									// cuando se termina una conversion
									
	/* Valores del Control Register (CR) */
	ADC0->CR = 	(1 << ADC_CHANNEL)	// Seleccion del canal 1 del ADC0
				| (45 << 8)			// Valor + 1 que se usa para dividir el clock
									// Debe ser tal de garantizar un reloj menor o
									// igual a 4.5 MHz
				| (0 << 16)			// Modo burst
				| (0 << 17)			// Cantidad de ciclos de reloj para la conversion
				| (0 << 20)			// Reservado
				| (1 << 21)			// Power on
				| (0 << 22)			// Reservado
				| (1 << 24)			// Iniciar el AD
				| (0 << 27)			// 
				| (0 << 28);		// Reservado

}

/* Interrupt routine for ADC example */
static void Adquisicion(void)
{
	/* Habilitacion de la interrupcion del ADC */
	NVIC_EnaIRQ(ADC0_IRQn);

	ADC0->INTEN |= (1UL << ADC_CHANNEL);	// Habilitacion de la interrupcion generada
											// cuando se termina una conversion
		
	ADC_Interrupt_Done_Flag = 1;
	while (1) {
		if (ADC_Interrupt_Done_Flag == 1) {
			ADC_Interrupt_Done_Flag = 0;
			ADC_ID->CR |= (1 << 24);		// Inicio de una adquisicion
		}
	}

	/* Deshabilitacion de la interrupcion del ADC */
	NVIC_DesIRQ(ADC0_IRQn);
}

// Funcion principal
int main (void)
{
	// Inicio de la UART para debugging
	UART_Init();
	
	SCU->SFSP[6][1] = ((2 << 3) | SCU_MODE_FUNC0); 	// P1_0, GPIO0[4], TEC_1

	GPIO_SetPinDIROutput(GPIO_PORT, 3, 0);
	GPIO_PORT->SET[3] |= 1 << 0;

	sprintf_mio(aux, "\r\n*** Inicio de la adquisicion %d ***\r\n\r\n", ADC_CHANNEL);
	DEBUGSTR(aux);

	// Configuracion del ADC
	ADC_Config();
	
	Adquisicion();

	return 0;
}
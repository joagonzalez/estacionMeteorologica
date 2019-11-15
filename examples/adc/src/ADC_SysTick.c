#include "ADC_SysTick.h"

#define ADC_CHANNEL 1
#define ADC_MIO ADC0
#define LEDS_MASK		(SCU_MODE_DES | SCU_MODE_EZI)

unsigned short  ADC_BUF = 0;

static char aux[256];	// para debugging (usada por printf)
int i = 0;

volatile int ticks = 0;		// cuenta las veces que el Systtick llego a cero
volatile int ledStatus = 0; // estado del led indicador de toma de muestra
volatile int delay_ms = 50;	// Control de la velocidad de la toma de muestras

unsigned int tmp;

void SysTick_Handler(void){

	ticks++;
}

void SysTickConfig(void){
	
	_SysTick->LOAD = 0x0D4C0;								// Set reload register
	//_SysTick->LOAD = _SysTick->CALIB & 0x00FFFFFF;	// Set reload register
													// CALIB se utiliza para obtener un timing
													// de 10 ms (100 Hz)
	_SysTick->VAL  = 0;								// Se limpia el valor
	_SysTick->CTRL = (0 << 2) 	|	// Se usa el reloj del sistema (204 MHz)
					 (1 << 1)	|	// Habilitacion de la interrupcion cuando llega a 0
					 (1 << 0);		// Habilitacion del contador	

}

static void ADC_Config(void) {

	ADC0->CR |=	(1 << ADC_CHANNEL) |		// Seleccion canal ADC0
				(231 << 8) |	// ADC0 clkdiv (maximo = 255) => Freq = 204MHz / (11 * (clkdiv + 1))
				(0 << 16) |		// Burst mode => Repeated conversions
				(0 << 17) |		// 10 bits resolution
				(1 << 21) |		// Power on
				(0 << 24) |		// Conversion start => Burst controlled (not software)
				(0 << 27) ;		// Start signal edge => Burst => Not significant bit
				
}


// Funcion principal
int main (void)
{
	UART_Init();					// Inicio de la UART
	Config_LEDS(LEDS_MASK);			// Configuracion de los leds
//	SysTick_Config(204000000/100);	// Establecimiento de la cuenta del SysTick
	SysTickConfig();
	ADC_Config();					// configuracion del ADC
	LED_ON(LED1);					// indicacion de encendido

	while (1){
		
		if (ticks == delay_ms){
			ticks = 0;
			if (ledStatus){
				ledStatus = 0;		// Cambia al otro estado
				LED_OFF(LED2);		// Apaga el led
			}
			else{
				ledStatus = 1;		// Cambia al otro estado
				LED_ON(LED2);		// Enciende el led
				ADC0->CR = (1 << 24) | (ADC0->CR);
				while ((ADC0->STAT && 0b1000) == 0){}
				ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
				sprintf_mio(aux, "%d\r\n", ADC_BUF);
				DEBUGSTR(aux);
			}
		}
	}

	return 0;
}
// FUNCIONANDO CANALES [1,2,3]
#include "ADC_SysTick.h"
#include <math.h>

#define ADC_CHANNEL2 2
#define ADC_CHANNEL1 1
#define ADC_MIO ADC0
#define LEDS_MASK		(SCU_MODE_DES | SCU_MODE_EZI)

unsigned short  ADC_BUF = 0;
unsigned short  ADC_CHANNEL = 2;
unsigned short ADC_MASK = 0b0000;
static char aux[256];	// para debugging (usada por printf)
int i = 0;

volatile int ticks = 0;		// cuenta las veces que el Systtick llego a cero
volatile int ledStatus = 0; // estado del led indicador de toma de muestra
volatile int delay_ms = 2000;	// Control de la velocidad de la toma de muestras

//steinhart-hart coeficients for thermistor
float c1 = 0.001129148;
float c2 = 0.000234125;
float c3 = 0.0000000876741; 

float R1 = 10000; // value of R1 on board
float logR2, R2, T, temperature;

unsigned int tmp;

void SysTick_Handler(void){

	ticks++;
}

void SysTickConfig(void){
	
	_SysTick->LOAD = 0x31CE0;							// Set reload register LOAD = 204MHz (Hz) * Time (s) = 204.000.000 * 0.001 (1ms)
	//_SysTick->LOAD = _SysTick->CALIB & 0x00FFFFFF;	// Set reload register
													// CALIB se utiliza para obtener un timing
													// de 10 ms (100 Hz)
	_SysTick->VAL  = 0;								// Se limpia el valor
	_SysTick->CTRL = (1 << 2) 	|	// Se usa el reloj del sistema (204 MHz)
					 (1 << 1)	|	// Habilitacion de la interrupcion cuando llega a 0
					 (1 << 0);		// Habilitacion del contador	

}

static void ADC_Config(void) {

	ADC0->CR |=	(0 << ADC_CHANNEL) |		// Seleccion canal ADC0
				(0 << 0) |
				(231 << 8) |	// ADC0 clkdiv (maximo = 255) => Freq = 204MHz / (11 * (clkdiv + 1))
				(0 << 16) |		// Burst mode => Repeated conversions
				(0 << 17) |		// 10 bits resolution
				(1 << 21) |		// Power on
				(0 << 24) |		// Conversion start => Burst controlled (not software)
				(0 << 27) ;		// Start signal edge => Burst => Not significant bit				
	
	sprintf_mio(aux, "Initial config ADC->CR config channel: %d\r\n", ADC0->CR);
	DEBUGSTR(aux);
}

static void ADC__Channel_Config(int channel) {

	ADC0->CR = (0x00000000 << 0); // reset del registro CR

	ADC0->CR |=	 (1 << channel) |		// Seleccion canal ADC0
			(231 << 8) |	// ADC0 clkdiv (maximo = 255) => Freq = 204MHz / (11 * (clkdiv + 1)) = 80khz
			(0 << 16) |		// Burst mode => Repeated conversions
			(0 << 17) |		// 10 bits resolution
			(1 << 21) |		// Power on
			(0 << 24) |		// Conversion start => Burst controlled (not software)
			(0 << 27) ;		// Start signal edge => Burst => Not significant bit

	sprintf_mio(aux, "ADC0->CR del channel %d: %d\r\n", channel, ADC0->CR);
	DEBUGSTR(aux);			
}

static float volt_to_degrees(unsigned short measurement, unsigned short channel) {
	R2 = R1 * (1023.0 / (float) measurement - 1.0); //calculate resistance on thermistor
	logR2 = log(R2);
	T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
	T = T - 273.15 - 20; //convert Kelvin to Celcius

	sprintf_mio(aux, "Temperature measured in channel %d: %d [°C]\r\n", channel, (unsigned short) T);
	DEBUGSTR(aux);

	return (unsigned short) T;			
}

static int adc_to_volt(unsigned short measurement){
	float result = 0;

	result = ((float) measurement * 3.3) / (1024.0);

	sprintf_mio(aux, "ADC measurement: %d was converted to: %d [V]\r\n", measurement, (unsigned short) result);
	DEBUGSTR(aux);
	
	return (unsigned short) result;

}

// Funcion principal
int main (void)
{
	UART_Init();					// Inicio de la UART
	Config_LEDS(LEDS_MASK);			// Configuracion de los leds
//	SysTick_Config(204000000/100);	// Establecimiento de la cuenta del SysTick
	SysTickConfig();
	ADC_Config();					// configuracion del ADC
	LED_ON(LEDB);					// indicacion de encendido

	sprintf_mio(aux, "##### COMIENZA CAPTURA DE DATOS #####\n\r");
	DEBUGSTR(aux);	
	while (1){
		
		if (ticks == delay_ms){
			ticks = 0;
			if (ledStatus){
				ledStatus = 0;		// Cambia al otro estado
				LED_OFF(LEDB);		// Apaga el led
			}
			else{
				ledStatus = 1;		// Cambia al otro estado
				LED_ON(LEDB);		// Enciende el led

				ADC0->CR = (1 << 24) | (ADC0->CR); // empezar lectura

				// poner esto en funcion aparte status_bit_config()
				if(ADC_CHANNEL == 2){
					ADC_MASK = 0b0100; // lectura canal 2
				}else if(ADC_CHANNEL == 1){
					ADC_MASK = 0b0010; // lectura canal 1
				}else{
					ADC_MASK = 0b1000; // lectura canal 3
				}

				while ((ADC0->STAT && ADC_MASK) == 0){} // esperar a que lectura ok

				if(ADC_CHANNEL == 2){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) 2);
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF);
					sprintf_mio(aux, "CHANNEL 2: %d | CHANNEL 2 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);
				}else if(ADC_CHANNEL == 1){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) 1); // enviamos adc channel info
					// UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF); // enviamos dato channel 1
					sprintf_mio(aux, "CHANNEL 1: %d | CHANNEL 1 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);
					// ADC_BUF = adc_to_volt(ADC_BUF);
					adc_to_volt(ADC_BUF);
					temperature = volt_to_degrees(ADC_BUF, ADC_CHANNEL);
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) temperature);

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);
				}else if(ADC_CHANNEL == 3){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) 3);
					UART_SendByte(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF);
					sprintf_mio(aux, "CHANNEL 3: %d | CHANNEL 3 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);	

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);			
				}

				if (ADC_CHANNEL == 1){
					ADC_CHANNEL = 2;
					ADC__Channel_Config(ADC_CHANNEL);
					
				}else if(ADC_CHANNEL == 2){
					ADC_CHANNEL = 3;
					ADC__Channel_Config(ADC_CHANNEL);					
				}else if(ADC_CHANNEL == 3){
					ADC_CHANNEL = 1;
					ADC__Channel_Config(ADC_CHANNEL);	
				}
			}
		}
	}

	return 0;
}

// Trabajo practico Final
// Materia: Electronica Digital II - 2019 (ECyT - UNSAM)
//
// Docentes:
//	- Sagreras Miguel
//	- Alvarez Nicolas
// Alumnos:
// 	- Gonzalez Joaquin - joagonzalez@gmail.com
// 	- Pedraza Sebastian - sebastianpedraza2002@yahoo.com.ar

// Librerias
#include "funciones.h"

// Definicion de variables
unsigned short ADC_BUF = 0;
unsigned short ADC_CHANNEL = 2;
unsigned short ADC_MASK = 0b0000;
static char aux[256];	// para debugging (usada por printf)
int i = 0;

volatile int ticks = 0;		// cuenta las veces que el Systtick llego a cero
volatile int ledStatus = 0; // estado del led indicador de toma de muestra
volatile int delay_ms = 2000;	// Control de la velocidad de la toma de muestras
float temperature;

// Funcion principal
int main (void)
{
	uart_config(CIAA_BOARD_UART_USB);	// Configuracion UART USB (2)
	uart_config(CIAA_BOARD_UART_RS232);	// Configuracion RS-232 (3)
	systick_config();			    	// Configuracion SysTick
	adc_config(0);					    // configuracion ADC

	config_leds(LEDS_MASK);			// Configuracion GPIO leds
	led_on(LEDB);					// Encendido led

	begin_telemetry_message();		// Mensaje inicial

	// Bucle principal	
	while (1){
		
		if (ticks == delay_ms){
			ticks = 0;
			if (ledStatus){
				ledStatus = 0;		// Cambia al otro estado
				led_off(LEDB);		// Apaga el led
			}
			else{
				ledStatus = 1;		// Cambia al otro estado
				led_on(LEDB);		// Enciende el led

				ADC0->CR = (1 << 24) | (ADC0->CR); // empezar lectura

				// Dependiendo el ADC channel a leer en esta iteracion se adapta la mascara para checkear status
				ADC_MASK = status_bit_config(ADC_CHANNEL);

				while ((ADC0->STAT && ADC_MASK) == 0){} // esperar a que lectura ok

				if(ADC_CHANNEL == 2){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) 2);
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF);
					
					blink_delay(LED2, 5000000);


					sprintf_mio(aux, "CHANNEL 2: %d | CHANNEL 2 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);
				}else if(ADC_CHANNEL == 1){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) 1); // enviamos adc channel info
					// uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF); // enviamos dato channel 1
					sprintf_mio(aux, "CHANNEL 1: %d | CHANNEL 1 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);
					// ADC_BUF = adc_to_volt(ADC_BUF);
					adc_to_volt(ADC_BUF);
					temperature = volt_to_degrees(ADC_BUF, ADC_CHANNEL);
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) temperature);

					blink_delay(LED1, 5000000);

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);
				}else if(ADC_CHANNEL == 3){
					ADC_BUF = (ADC0->DR[ADC_CHANNEL] >> 6) & 0x3FF;
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) 3);
					uart_enviar_datos(CIAA_BOARD_UART_RS232, (uint8_t) ADC_BUF);
					sprintf_mio(aux, "CHANNEL 3: %d | CHANNEL 3 CAST: %d\r\n", ADC_BUF, (uint8_t) ADC_BUF);
					DEBUGSTR(aux);	

					blink_delay(LED3, 5000000);

					sprintf_mio(aux, "###########################################\r\n");
					DEBUGSTR(aux);			
				}

				if (ADC_CHANNEL == 1){
					ADC_CHANNEL = 2;
					adc_config(ADC_CHANNEL);
					
				}else if(ADC_CHANNEL == 2){
					ADC_CHANNEL = 3;
					adc_config(ADC_CHANNEL);					
				}else if(ADC_CHANNEL == 3){
					ADC_CHANNEL = 1;
					adc_config(ADC_CHANNEL);	
				}
			}
		}
	}

	return 0;
}

// Handlers
void SysTick_Handler(void){
	ticks++;
}
// Trabajo practico Final
// Materia: Electronica Digital II - 2019 (ECyT - UNSAM)
//
// Docentes:
//	- Sagreras Miguel
//	- Alvarez Nicolas
// Alumnos:
// 	- Gonzalez Joaquin - joagonzalez@gmail.com
// 	- Pedraza Sebastian - sebastianpedraza2002@yahoo.com.ar

#include <math.h>
#include "estructuras.h"  

/************************************************************************************
 *	Funciones UART
 ************************************************************************************/

 void uart_config(USART_T *pUART){

	// Habilitacion del reloj para la UART. El reloj base debe estar habilitado
	CCU1->CLKCCU[2].CFG = (1 << 0) | (1 << 1) | (1 << 2);
   

	// Chip_UART_SetupFIFOS
	pUART->FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);
	// (1 << 0) | (1 << 1) | (1 << 2)

	// Disable Tx
	pUART->TER2 = 0;

	// Disable interrupts
	pUART->IER = 0;
	// Set LCR to default state
	pUART->LCR = 0;
	// Set ACR to default state
	pUART->ACR = 0;
    // Set RS485 control to default state
	pUART->RS485CTRL = 0;
	// Set RS485 delay timer to default state
	pUART->RS485DLY = 0;
	// Set RS485 addr match to default state/
	pUART->RS485ADRMATCH = 0;
	// Set Modem Control to default state
	pUART->MCR = 0;
	//8N1, with DLAB disabled
 	// El LCR determina el formato del carácter de los datos que se va a transmitir o recibir
	pUART->LCR =  (3 << 0)| //Largo de la palabra: 8 bits
			 	  (0 << 2)| //Bit de parada: 1
				  (0 << 3); //Paridad desactivada
	// Disable fractional divider
	pUART->FDR = 0x10;	//1 <= MULVAL <=15

	// Set Baud rate
	unsigned int clkin;

	if(pUART == CIAA_BOARD_UART_USB){
		clkin = Chip_Clock_GetRate(CLK_APB2_UART2);
	}else if(pUART == CIAA_BOARD_UART_RS232){
		clkin = Chip_Clock_GetRate(CLK_APB2_UART3);
 	}

	int div = clkin / (SYSTEM_BAUD_RATE * 16);
	// /* High and low halves of the divider */
	int divh = div / 256;
	int divl = div - (divh * 256);

	//Divisor de palabras
	pUART->LCR |= (1 << 7); //Habilitacion del divisor

	pUART->DLL = (uint32_t) divl; //El byte menos significativo del valor del divisor de la tasa de baudios.
	pUART->DLM = (uint32_t) divh; //El byte más significativo del valor del divisor de la tasa de baudios.
	
	pUART->LCR &= ~(1 << 7); //Desactivacion del divisor

   	//Modify FCR (FIFO Control Register)
	pUART->FCR =  (1 << 0)| // Habilitacion de FIFO de la UART
	      		  (1 << 1)| // Reset de la FIFO del Receptor (Rx)
				  (1 << 2); // Reset de la FIFO del Transmisor (Tx)

	//Habilitacion del transmisor (Tx) de la UART 
    pUART->TER2 = (1 << 0);

	if(pUART == CIAA_BOARD_UART_USB){
		/* P7_1: UART2_TXD */
		SCU->SFSP[7][1] = (MD_PDN | SCU_MODE_FUNC6);
		/* P7_2: UART2_RXD */
		SCU->SFSP[7][2] = (MD_PLN | SCU_MODE_EZI | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6);	
	}else if(pUART == CIAA_BOARD_UART_RS232){
		/* P2_4: UART3_RXD */
		SCU->SFSP[2][4] = (MD_PLN | SCU_MODE_EZI | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2);
		/* P2_3: UART3_TXD */
		SCU->SFSP[2][3] = (MD_PDN | SCU_MODE_FUNC2);
	}

	// Enable UART Rx Interrupt
	pUART->IER |= (1 << 0); //Activa la interrupción de Recibir Datos Disponibles para el USART (table 927)
   
}

void uart_enviar_datos(USART_T *pUART, unsigned char data){
	pUART->THR = (unsigned int) data;
}

/************************************************************************************
 *	Funciones ADC
************************************************************************************/

void adc_config(int channel){
	static char aux[256];	// para debugging (usada por printf)

	ADC0->CR = (0x00000000 << 0); // reset del registro CR

	ADC0->CR |=	 (1 << channel) |		// Seleccion canal ADC0
                 (231 << 8) |	// ADC0 clkdiv (maximo = 255) => Freq = 204MHz / (11 * (clkdiv + 1)) = 80khz
                 (0 << 16) |		// Burst mode => Repeated conversions
                 (0 << 17) |		// 10 bits resolution
                 (1 << 21) |		// Power on
                 (0 << 24) |		// Conversion start => Burst controlled (not software)
                 (0 << 27) ;		// Start signal edge => Burst => Not significant bit

	sprintf_mio(aux, "Configuring ADC0->CR of channel %d: %d\r\n", channel, ADC0->CR);
	DEBUGSTR(aux);			
}

unsigned short status_bit_config(int channel){
	unsigned short ADC_MASK;

	if(channel == 2){
		ADC_MASK = 0b0100; // lectura canal 2
	}else if(channel == 1){
		ADC_MASK = 0b0010; // lectura canal 1
	}else{
		ADC_MASK = 0b1000; // lectura canal 3
	}

	return ADC_MASK;

}

/************************************************************************************
 *	Funciones SysTick
 ************************************************************************************/
 
 void systick_config(void){
	_SysTick->LOAD = 0x31CE0;		// Reload register - LOAD = 204MHz (Hz) * Time (s) = 204.000.000 * 0.001 (1ms)
	_SysTick->VAL  = 0;				// Inicializar current VAL en 0
	_SysTick->CTRL = (1 << 2) 	|	// Se usa el reloj del sistema (204 MHz)
					 (1 << 1)	|	// Habilitacion de la interrupcion cuando llega a 0
					 (1 << 0);		// Habilitacion del contador	

}

/************************************************************************************
 *	Funciones Sensores
 ************************************************************************************/

float volt_to_degrees(unsigned short measurement, unsigned short channel) {
	static char aux[256];	// para debugging (usada por printf)

	R2 = R1 * (1023.0 / (float) measurement - 1.0); //calculate resistance on thermistor
	logR2 = log(R2);
	T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)) - 20; // temperature in Kelvin
	T = T - 273.15; //convert Kelvin to Celcius

	sprintf_mio(aux, "Temperature measured in channel %d: %d [°C]\r\n", channel, (unsigned short) T);
	DEBUGSTR(aux);

	return (unsigned short) T;			
}

int adc_to_volt(unsigned short measurement){
	static char aux[256];	// para debugging (usada por printf)
	float result = 0;

	result = ((float) measurement * 3.3) / (1024.0);

	sprintf_mio(aux, "ADC measurement: %d was converted to: %d [V]\r\n", measurement, (unsigned short) result);
	DEBUGSTR(aux);
	
	return (unsigned short) result;

}

/************************************************************************************
 *	Funciones GPIO
 ************************************************************************************/

void GPIO_SetPinDIROutput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin) {
	pGPIO->DIR[puerto] |= 1 << pin;
}

void GPIO_SetPinDIRInput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin) {
	pGPIO->DIR[puerto] &= ~(1 << pin);
}

void led_on(enum LEDS led) {
	
	switch(led) {
		case 0:
			GPIO_PORT->SET[0] |= (1 << 14);
			break;
		case 1:
			GPIO_PORT->SET[1] |= (1 << 11);
			break;
		case 2:
			GPIO_PORT->SET[1] |= 1 << 12;
			break;
		case 3:
			GPIO_PORT->SET[5] |= 1 << 0;
			break;
		case 4:
			GPIO_PORT->SET[5] |= 1 << 1;
			break;
		case 5:
			GPIO_PORT->SET[5] |= 1 << 2;
			break;
	}
}

void led_off(enum LEDS led) {
	
	switch(led) {
		case 0:
			GPIO_PORT->CLR[0] |= (1 << 14);
			break;
		case 1:
			GPIO_PORT->CLR[1] |= (1 << 11);
			break;
		case 2:
			GPIO_PORT->CLR[1] |= 1 << 12;
			break;
		case 3:
			GPIO_PORT->CLR[5] |= 1 << 0;
			break;
		case 4:
			GPIO_PORT->CLR[5] |= 1 << 1;
			break;
		case 5:
			GPIO_PORT->CLR[5] |= 1 << 2;
			break;
	}
}

void config_leds(int MASK) {
	// Configuracion de los pines (LED1, LED2, LED3, LEDR, LEDG y LEDB) como GPIO
	// (Registro de configuracion, pag 405 / Tabla 191)
	SCU->SFSP[2][10] = (MASK | SCU_MODE_FUNC0); // P2_10, GPIO0[14], LED1
	SCU->SFSP[2][11] = (MASK | SCU_MODE_FUNC0); // P2_11, GPIO1[11], LED2
	SCU->SFSP[2][12] = (MASK | SCU_MODE_FUNC0); // P2_12, GPIO1[12], LED3
	SCU->SFSP[2][0] = (MASK | SCU_MODE_FUNC4); 	// P2_0,  GPIO5[0],  LEDR
	SCU->SFSP[2][1] = (MASK | SCU_MODE_FUNC4); 	// P2_1,  GPIO5[1],  LEDG
	SCU->SFSP[2][2] = (MASK | SCU_MODE_FUNC4); 	// P2_2,  GPIO5[2],  LEDB
	
	// Configuracion de los pines (LED1, LED2, LED3, LEDR, LEDG y LEDB) como salida
	// (Registro de direccion, pag 455 -> Tabla 261)
	GPIO_SetPinDIROutput(GPIO_PORT, 0, 14);
	GPIO_SetPinDIROutput(GPIO_PORT, 1, 11);
	GPIO_SetPinDIROutput(GPIO_PORT, 1, 12);
	GPIO_SetPinDIROutput(GPIO_PORT, 5, 0);
	GPIO_SetPinDIROutput(GPIO_PORT, 5, 1);
	GPIO_SetPinDIROutput(GPIO_PORT, 5, 2);
	
}

void config_botones(int MASK) {
	SCU->SFSP[1][0] = (MASK | SCU_MODE_FUNC0); 	// P1_0, GPIO0[4], TEC_1
	SCU->SFSP[1][1] = (MASK | SCU_MODE_FUNC0); 	// P1_1, GPIO0[8], TEC_2
	SCU->SFSP[1][2] = (MASK | SCU_MODE_FUNC0); 	// P1_2, GPIO0[9], TEC_3
	SCU->SFSP[1][6] = (MASK | SCU_MODE_FUNC0);	// P1_6, GPIO1[9], TEC_4

	GPIO_SetPinDIRInput(GPIO_PORT, 0, 4);
	GPIO_SetPinDIRInput(GPIO_PORT, 0, 8);
	GPIO_SetPinDIRInput(GPIO_PORT, 0, 9);
	GPIO_SetPinDIRInput(GPIO_PORT, 1, 9);
	
}

/************************************************************************
*Cambio de estado de LEDS
 ************************************************************************/
void GPIO_SetPinToggle(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin)
{
	pGPIO->NOT[puerto] = (1 << pin); 
}

/************************************************************************
*Funcion para configuracion de las Teclas
 ************************************************************************/
void teclas_config(SCU_T *pSCU)
{
		pSCU->SFSP[1][0] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][1] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][2] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][6] = (0 << 0) | (1 << 4) | (1 << 6);
}

/************************************************************************
*Funcion para declarar las Teclas como entrada
 ************************************************************************/
void teclas_in(GPIO_T *pGPIO){
    pGPIO->DIR[0] |= (0 << 4) | (0 << 8) | (0 << 9);
    pGPIO->DIR[1] |= (0 << 9); 
}

/************************************************************************************
 *	Funciones genericas
 ************************************************************************************/

void retardo(int base){
    int i;
    for(i=0;i<base;i++){}
}

void blink_delay(enum LEDS led, int delay){
	led_on(led);
	retardo(delay);
	led_off(led);
}

/************************************************************************************
 *	Funciones debug
 ************************************************************************************/

static void begin_telemetry_message(void){
	static char aux[256];

	sprintf_mio(aux, "#####################################\n\r");
	DEBUGSTR(aux);
	sprintf_mio(aux, "##### COMIENZA CAPTURA DE DATOS #####\n\r");
	DEBUGSTR(aux);
	sprintf_mio(aux, "#####################################\n\r");
	DEBUGSTR(aux);
}

static void printchar(char **str, int c){
	// extern int putchar(int c);
	
	if (str) {
		**str = c;
		++(*str);
	}
	else 
		uart_enviar_datos(CIAA_BOARD_UART_USB, c);
		// uart_enviar_datos(CIAA_BOARD_UART_RS232, c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

 static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print(char **out, int *varg)
{
	register int width, pad;
	register int pc = 0;
	register char *format = (char *)(*varg++);
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = *((char **)varg++);
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, *varg++, 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, *varg++, 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, *varg++, 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = *varg++;
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	return pc;
}

/* assuming sizeof(void *) == sizeof(int) */

int printf_mio(const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(0, varg);
}

int sprintf_mio(char *out, const char *format, ...)
{
	register int *varg = (int *)(&format);
	return print(&out, varg);
}


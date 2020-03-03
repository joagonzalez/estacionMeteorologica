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

 void uart_config(void){

	// *****************************************************************************
	// Habilitacion del reloj para la UART. El reloj base debe estar habilitado
	// Chip_Clock_EnableOpts
	CCU1->CLKCCU[2].CFG = (1 << 0) | (1 << 1) | (1 << 2);;
   
	// Chip_UART_SetupFIFOS
	USART2->FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);
	USART3->FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);

	// Disable Tx
	USART2->TER2 = 0;
	USART3->TER2 = 0;

	// Disable interrupts
	USART2->IER = 0;
	USART3->IER = 0;
	// Set LCR to default state
	USART2->LCR = 0;
	USART3->LCR = 0;
	// Set ACR to default state
	USART2->ACR = 0;
	USART3->ACR = 0;
    // Set RS485 control to default state
	USART2->RS485CTRL = 0;
	USART3->RS485CTRL = 0;
	// Set RS485 delay timer to default state
	USART2->RS485DLY = 0;
	USART3->RS485DLY = 0;
	// Set RS485 addr match to default state/
	USART2->RS485ADRMATCH = 0;
	USART3->RS485ADRMATCH = 0;
	// Clear MCR (solo para USART1)
	// Set Modem Control to default state
	USART2->MCR = 0;
	USART3->MCR = 0;
	// Dummy Reading to Clear Status
//	int tmp = USART2->MSR;
	// Default 8N1, with DLAB disabled
	USART2->LCR = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
	USART3->LCR = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
	// Disable fractional divider
	USART2->FDR = 0x10;
	USART3->FDR = 0x10;


	// *****************************************************************************
	// Set Baud rate
	//Chip_UART_SetBaud((LPC_USART_T *)USART2, SYSTEM_BAUD_RATE);
	unsigned int clkin_2 = Chip_Clock_GetRate(CLK_APB2_UART2);
	unsigned int clkin_3 = Chip_Clock_GetRate(CLK_APB2_UART3);
	
	int div_2 = clkin_2 / (SYSTEM_BAUD_RATE * 16);

	// /* High and low halves of the divider */
	int divh_2 = div_2 / 256;
	int divl_2 = div_2 - (divh_2 * 256);

	int div_3 = clkin_3 / (SYSTEM_BAUD_RATE * 16);

	// /* High and low halves of the divider */
	int divh_3 = div_3 / 256;
	int divl_3 = div_3 - (divh_3 * 256);

	// Chip_UART_EnableDivisorAccess(CIAA_BOARD_UART);
	USART2->LCR |= UART_LCR_DLAB_EN;
	USART3->LCR |= UART_LCR_DLAB_EN;
	// Chip_UART_SetDivisorLatches(CIAA_BOARD_UART, divl, divh);
	USART2->DLL = (uint32_t) divl_2;
	USART2->DLM = (uint32_t) divh_2;

	USART3->DLL = (uint32_t) divl_3;
	USART3->DLM = (uint32_t) divh_3;

	// Chip_UART_DisableDivisorAccess(CIAA_BOARD_UART);
	USART2->LCR &= ~UART_LCR_DLAB_EN;
	USART3->LCR &= ~UART_LCR_DLAB_EN;

	/* Fractional FDR alreadt setup for 1 in UART init */
	// return clkin / div;

   // *****************************************************************************
   
	//Modify FCR (FIFO Control Register)
	// Chip_UART_SetupFIFOS(CIAA_BOARD_UART, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
	USART2->FCR = (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
	USART3->FCR = (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
	// Enable UART Transmission
	// Chip_UART_TXEnable(CIAA_BOARD_UART);
    USART2->TER2 = UART_TER2_TXEN;
	USART3->TER2 = UART_TER2_TXEN;

	// Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              
	SCU->SFSP[7][1] = (MD_PDN | SCU_MODE_FUNC6);						/* P7_1: UART2_TXD */
	SCU->SFSP[2][3] = (MD_PDN | SCU_MODE_FUNC2);						/* P2_3: UART3_TXD */

	//Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); 
	SCU->SFSP[7][2] = (MD_PLN | SCU_MODE_EZI | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6);	/* P7_2: UART2_RXD */
	SCU->SFSP[2][4] = (MD_PLN | SCU_MODE_EZI | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2);	/* P2_4: UART3_RXD */
	
	// Enable UART Rx Interrupt
	// Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RBRINT);   //Receiver Buffer Register Interrupt
	USART2->IER |= _UART_IER_RBRINT;
	USART3->IER |= _UART_IER_RBRINT;
   
}

void UART_SendByte(USART_T *pUART, unsigned char data){
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

	sprintf_mio(aux, "ADC0->CR del channel %d: %d\r\n", channel, ADC0->CR);
	DEBUGSTR(aux);			
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
	T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
	T = T - 273.15 - 20; //convert Kelvin to Celcius

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

void LED_ON(enum LEDS led) {
	
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

void LED_OFF(enum LEDS led) {
	
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

void Config_Botones(int MASK) {
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
 * GPIO Interrupt Pin Select
 * PortSel	: Numero de interrupcion de GPIO (0 a 7)
 * PortNum	: GPIO port number interrupt, should be: 0 to 7
 * PinNum	: GPIO pin number Interrupt , should be: 0 to 31
 ************************************************************************/
void SCU_GPIOIntPinSel(unsigned char PortSel, unsigned char PortNum, unsigned char PinNum){
	int despl = (PortSel & 3) << 3;
	unsigned int val = (((PortNum & 0x7) << 5) | (PinNum & 0x1F)) << despl;
	SCU->PINTSEL[PortSel >> 2] = (SCU->PINTSEL[PortSel >> 2] & ~(0xFF << despl)) | val;
}

/************************************************************************
 * Establecimiento de la prioridad de una interrupcion
 ************************************************************************/
void NVIC_SetPri(IRQn_Type IRQn, unsigned int priority){
	if(IRQn < 0) {
	}
	else {
		_NVIC->IP[(unsigned int)(IRQn)] = ((priority << (8 - 2)) & 0xff);
	}
}

void NVIC_EnaIRQ(IRQn_Type IRQn){
	_NVIC->ISER[(unsigned int)((int)IRQn) >> 5] = (unsigned int)(1 << ((unsigned int)((unsigned int)IRQn) & (unsigned int)0x1F));
}

void NVIC_DesIRQ(IRQn_Type IRQn){
	_NVIC->ICER[(unsigned int)((int)IRQn) >> 5] = (1 << ((unsigned int) (IRQn) & 0x1F));
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
void TECLAS_CONFIG(SCU_T *pSCU)
{
		pSCU->SFSP[1][0] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][1] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][2] = (0 << 0) | (1 << 4) | (1 << 6);
		pSCU->SFSP[1][6] = (0 << 0) | (1 << 4) | (1 << 6);
}

/************************************************************************
*Funcion para declarar las Teclas como entrada
 ************************************************************************/
void TEC_IN(GPIO_T *pGPIO){
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
		UART_SendByte(CIAA_BOARD_UART_USB, c);
		// UART_SendByte(CIAA_BOARD_UART_RS232, c);
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


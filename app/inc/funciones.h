//Funciones para controlar LEDs EDU CIAA
#include "estructuras.h"

//Base en ciclos
void retardo(int base){
    int i;
    for(i=0;i<base;i++){}
}

//Registro DIR - Setea puerto como entrada o salida
void GPIO_SetPinDIROutput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin){
    pGPIO->DIR[puerto] |= (1 << pin);
}

void GPIO_SetPinDIRInput(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin){
    pGPIO->DIR[puerto] |= (0 << pin);
}

int GPIO_GetPinDir(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin){
    int state;
    state = pGPIO->DIR[puerto];
    (state >> pin) && 0x1;
    return(state);
}

//Registro DIR con mascara para manipular multiples bits a la vez
void GPIO_SetPortDIROutput(GPIO_T *pGPIO, unsigned char puerto, int mascara) {
	pGPIO->DIR[puerto] |= mascara;
}

void GPIO_SetPortDIRInput(GPIO_T *pGPIO, unsigned char puerto, int mascara) {
	pGPIO->DIR[puerto] &= ~(mascara);
}

int GPIO_GetPortDIR(GPIO_T *pGPIO, unsigned char puerto){
	int state;
    state = pGPIO->DIR[puerto];
	return(state);
}

//SET y CLEAR de los puertos GPIO
void GPIO_SetPin(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin, int valor){
    pGPIO->SET[puerto] |= (valor << pin);        
}

void GPIO_ClearPin(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin, int valor){
    pGPIO->CLR[puerto] |= (valor << pin);
}

int GPIO_GetPinState(GPIO_T *pGPIO, unsigned char puerto, unsigned char pin){
    int state;
    state = pGPIO->PIN[puerto];
    return(state);
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

//Secuencia de leds predefinida en funcion - Heredada de practica 3
void GPIO_Secuencia(){

while(1){
		//APAGAR LEDS 
        
        LED_OFF(0);
        retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,5,0,1);
        LED_OFF(1);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,5,1,1);
        LED_OFF(2);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,5,2,1);

        LED_OFF(3);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,0,14,1);
        LED_OFF(4);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,1,11,1);       
        LED_OFF(5);
        retardo(1000000); //Delay 0.5 seg
        //GPIO_ClearPin(GPIO_PORT,1,12,1);

		//ENCENDER LEDS 
        LED_ON(0);
        retardo(1000000); //Delay 0.5 seg
        //GPIO_SetPin(GPIO_PORT,5,0,1);
		LED_ON(1);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_SetPin(GPIO_PORT,5,1,1);
		LED_ON(2);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_SetPin(GPIO_PORT,5,2,1);

        LED_ON(3);
		retardo(1000000); //Delay 0.5 seg
		//GPIO_SetPin(GPIO_PORT,0,14,1);
        LED_ON(4);
		retardo(1000000); //Delay 0.5 seg
		//GPIO_SetPin(GPIO_PORT,1,12,1);
        LED_ON(5);
		retardo(1000000); //Delay 0.5 seg
        //GPIO_SetPin(GPIO_PORT,1,11,1);
	}
}


void Config_LEDS(int MASK) {
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

void UART_Init(void){

   //Initialize peripheral
   Chip_UART_Init(CIAA_BOARD_UART);
	
   // Set Baud rate
   Chip_UART_SetBaud(CIAA_BOARD_UART, SYSTEM_BAUD_RATE);

   //Modify FCR (FIFO Control Register)
   Chip_UART_SetupFIFOS(CIAA_BOARD_UART, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);

   // Enable UART Transmission
   Chip_UART_TXEnable(CIAA_BOARD_UART);

   Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              /* P7_1: UART2_TXD */
   Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); /* P7_2: UART2_RXD */

   //Enable UART Rx Interrupt
   Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RBRINT);   //Receiver Buffer Register Interrupt
   
   // Enable UART line status interrupt
   //Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RLSINT ); //LPC43xx User manual page 1118
   NVIC_SetPriority(USART2_IRQn, 6);
   
   // Enable Interrupt for UART channel
//   NVIC_EnableIRQ(USART2_IRQn);
//	NVIC_EnaIRQ(USART2_IRQn);
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
	_NVIC->ISER[(unsigned int)((int)IRQn) >> 5] = (unsigned int)(1 << ((unsigned int)((int32_t)IRQn) & (unsigned int)0x1F));
}

void NVIC_DesIRQ(IRQn_Type IRQn){
	_NVIC->ISER[(unsigned int)((int)IRQn) >> 5] = (unsigned int)(0 << ((unsigned int)((int32_t)IRQn) & (unsigned int)0x1F));
}

void Secuencia_Tecla(bool TEC, int PUERTO){
	if(TEC==1 && PUERTO==0){
		LED_ON(3);
		LED_ON(4);
		LED_ON(5);
	}else if(TEC==0 && PUERTO==0){
		LED_OFF(3);
		LED_OFF(4);
		LED_OFF(5);
	}else if(TEC==1 && PUERTO==1){
		LED_ON(0);
	}else if(TEC==0 && PUERTO==1){
		LED_OFF(0);
	}else if(TEC==1 && PUERTO==2){
		LED_ON(1);
	}else if(TEC==0 && PUERTO==2){
		LED_OFF(1);
	}else if(TEC==1 && PUERTO==3){
		LED_ON(2);
	}else if(TEC==0 && PUERTO==3){
		LED_OFF(2);
	}
}


//Funcion de configuracion del ADC
unsigned int ADC_CONFIG(unsigned int canal)
		{
		ADC0->INTEN = 0; 				// Deshabilitacion de las interrupcion generada
										// cuando se termina una conversion
										
		/* Valores del Control Register (CR) */
		ADC0->CR = 	(1 << canal)	// Seleccion del canal del ADC0
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
		
//Funcion de interrupcion del ADC 
unsigned int obt_datos(unsigned int canal)
{
	/* Habilitacion de la interrupcion del ADC */
	
	NVIC_EnaIRQ(ADC0_IRQn);

	ADC0->INTEN |= (1UL << canal);	// Habilitacion de la interrupcion generada
											// cuando se termina una conversion	
	ADC_Interrupt_Done_Flag = 1;
	
	while (1) {
				if (ADC_Interrupt_Done_Flag == 1) 
				{
				ADC_Interrupt_Done_Flag = 0;
				ADC0->CR |= (1 << 24);		// Inicio de una adquisicion
				}
	}

	/* Deshabilitacion de la interrupcion del ADC */
	NVIC_DesIRQ(ADC0_IRQn);
}		

/* Impresion del valor adquirido y generacion de retardo */
static void send_ADC_UART(uint16_t data){
		sprintf_mio(debug, "Dato leido del ADC: %d\r\n", data);
		DEBUGSTR(debug);
		retardo(10000000);
}

void _Configuracion_IO(void){

    //Configuro pines (LEDs y teclas) como salida sin pull-up/pull-down con buffer habilitado
	Config_LEDS(SCU_MODE_DES | SCU_MODE_EZI);
	Config_Botones(SCU_MODE_DES | SCU_MODE_EZI);

	//Deshabilitación las resistencias de pull‐up/pull‐down del pin P4_4 y habilitación del DAC
	SCU->SFSP[4][4] = (SCU_MODE_DES | SCU_MODE_FUNC7);
	SCU->ENAIO[2] = 1;

    //Inicializo el PINTSEL (pag. 423 ARM NXP)
	SCU->PINTSEL[0]=0x0;
	
    //Configutro Interrupción para puertos GPIO
	//INTERRUPCION, PUERTO GPIO, Bit del PIN
	SCU_GPIOIntPinSel(0, 0, 4);	// TEC_1
	SCU_GPIOIntPinSel(1, 0, 8);	// TEC_2
	SCU_GPIOIntPinSel(2, 0, 9);	// TEC_3
	SCU_GPIOIntPinSel(3, 1, 9);	// TEC_4
	
	//Configuro interrupción por flanco descendente y la habilito
	GPIO_PIN_INT->ISEL=0x0;  //Edge Sensitive (pag. 457)
	GPIO_PIN_INT->IENF=0xF;  //Enable Falling Edge or Level interrupt (pag. 457)
	GPIO_PIN_INT->SIENF=0xF; //Enable Falling Edge or Level interrupt (pag. 458)
    
	//Configuro prioridad de la IRQ para GPIO
	//Estas interrupciones serán generadas por los GPIO y serán enviadas y gestionadas por NVIC
	//PIN Number IRQ + Prioridad
	NVIC_SetPri(PIN_INT0_IRQn,15);
	NVIC_SetPri(PIN_INT1_IRQn,15);
	NVIC_SetPri(PIN_INT2_IRQn,15);
	NVIC_SetPri(PIN_INT3_IRQn,15);
	//Se habilita IRQ en NVIC
	NVIC_EnaIRQ(PIN_INT0_IRQn);
	NVIC_EnaIRQ(PIN_INT1_IRQn);
	NVIC_EnaIRQ(PIN_INT2_IRQn);
	NVIC_EnaIRQ(PIN_INT3_IRQn);
}

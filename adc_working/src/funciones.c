/**********************************************************************************************
 * Funciones de configuracion de los diferentes perifericos
 * Solo se trabaja con acceso directo a los registros de configuracion
 * 
 * Materia: Electronica Digital II - 2016 (UNSAM)
 *
 * Documentacion:
 *    - UM10503 (LPC43xx ARM Cortex-M4/M0 multi-core microcontroller User Manual)
 *    - PINES UTILIZADOS DEL NXP LPC4337 JBD144 (Ing. Eric Pernia)
 **********************************************************************************************/

#include "ADC_SysTick.h"

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
 
void TEC_IN(GPIO_T *pGPIO)
{
    pGPIO->DIR[0] |= (0 << 4) | (0 << 8) | (0 << 9);
    pGPIO->DIR[1] |= (0 << 9); 
}

/************************************************************************
*Funcion para apagar los LEDS
 ************************************************************************/
 
 /*void LED_OFF(GPIO_T *pGPIO, unsigned char port, unsigned char pin)
{
			if (port == 0)
						{
						pGPIO->CLR[port] = (1 << pin);
						}
			if (port == 1)
						{
						pGPIO->CLR[port] = (1 << pin);
						}
			else
						{
						pGPIO->CLR[port] = (1 << pin);
						}
}*/

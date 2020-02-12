/**********************************************************************************************
 * Endendido de un led en la placa EDU-CIAA, sin la utilizacion de librerias
 * Solo se trabaja con acceso directo a los registros de configuracion
 * 
 * Materia: Electronica Digital II - 2016 (UNSAM)
 *
 * Documentacion:
 *    - UM10503 (LPC43xx ARM Cortex-M4/M0 multi-core microcontroller User Manual)
 *    - PINES UTILIZADOS DEL NXP LPC4337 JBD144 (Ing. Eric Pernia)
 **********************************************************************************************/

#include "ADC_SysTick.h"

void UART_Init(void){

	//Initialize peripheral

	//**************************************************************************
	// Chip_UART_Init(CIAA_BOARD_UART);
	
	// *****************************************************************************
	// Habilitacion del reloj para la UART. El reloj base debe estar habilitado
	// Chip_Clock_EnableOpts
	CCU1->CLKCCU[2].CFG = (1 << 0) | (1 << 1) | (1 << 2);;
   
	// Chip_UART_SetupFIFOS
	USART2->FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS);

	// Disable Tx
	USART2->TER2 = 0;

	// Disable interrupts
	USART2->IER = 0;
	// Set LCR to default state
	USART2->LCR = 0;
	// Set ACR to default state
	USART2->ACR = 0;
    // Set RS485 control to default state
	USART2->RS485CTRL = 0;
	// Set RS485 delay timer to default state
	USART2->RS485DLY = 0;
	// Set RS485 addr match to default state/
	USART2->RS485ADRMATCH = 0;
	// Clear MCR (solo para USART1)
	// Set Modem Control to default state
	USART2->MCR = 0;
	// Dummy Reading to Clear Status
//	int tmp = USART2->MSR;
	// Default 8N1, with DLAB disabled
	USART2->LCR = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
	// Disable fractional divider
	USART2->FDR = 0x10;


	// *****************************************************************************
	// Set Baud rate
	//Chip_UART_SetBaud((LPC_USART_T *)USART2, SYSTEM_BAUD_RATE);
	unsigned int clkin = Chip_Clock_GetRate(CLK_APB2_UART2);
	int div = clkin / (SYSTEM_BAUD_RATE * 16);

	// /* High and low halves of the divider */
	int divh = div / 256;
	int divl = div - (divh * 256);

	// Chip_UART_EnableDivisorAccess(CIAA_BOARD_UART);
	USART2->LCR |= UART_LCR_DLAB_EN;

	// Chip_UART_SetDivisorLatches(CIAA_BOARD_UART, divl, divh);
	USART2->DLL = (uint32_t) divl;
	USART2->DLM = (uint32_t) divh;

	// Chip_UART_DisableDivisorAccess(CIAA_BOARD_UART);
	USART2->LCR &= ~UART_LCR_DLAB_EN;

	/* Fractional FDR alreadt setup for 1 in UART init */
	// return clkin / div;

   // *****************************************************************************
   
	//Modify FCR (FIFO Control Register)
	// Chip_UART_SetupFIFOS(CIAA_BOARD_UART, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
	USART2->FCR = (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0);
		
	// Enable UART Transmission
	// Chip_UART_TXEnable(CIAA_BOARD_UART);
    USART2->TER2 = UART_TER2_TXEN;

	// Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);              
	SCU->SFSP[7][1] = (MD_PDN | SCU_MODE_FUNC6);						/* P7_1: UART2_TXD */

	//Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6); 
	SCU->SFSP[7][2] = (MD_PLN | SCU_MODE_EZI | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC6);	/* P7_2: UART2_RXD */

	// Enable UART Rx Interrupt
	// Chip_UART_IntEnable(CIAA_BOARD_UART,UART_IER_RBRINT);   //Receiver Buffer Register Interrupt
	USART2->IER |= _UART_IER_RBRINT;

   
	// Enable UART line status interrupt
	//Chip_UART_IntEnable(USART2,UART_IER_RLSINT ); //LPC43xx User manual page 1118
	NVIC_SetPriority(USART2_IRQn, 6);
   
	// Enable Interrupt for UART channel
	// NVIC_EnableIRQ(USART2_IRQn);
	// NVIC_EnaIRQ(USART2_IRQn);
}

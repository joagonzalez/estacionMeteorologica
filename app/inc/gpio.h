// Trabajo practico Final
// Materia: Electronica Digital II - 2019 (ECyT - UNSAM)
//
// Docentes:
//	- Sagreras Miguel
//	- Alvarez Nicolas
// Alumnos:
// 	- Gonzalez Joaquin - joagonzalez@gmail.com
// 	- Pedraza Sebastian - sebastianpedraza2002@yahoo.com.ar

enum LEDS {LED1, LED2, LED3, LEDR, LEDG, LEDB}; 

/************************************************************************************
 GPIO
 ************************************************************************************/

#define GPIO_PORT_BASE			0x400F4000	// direccion base del registro (pag. 455 / tabla 245)
#define GPIO_PORT0_B_OFFSET		0x0000		// tabla 259
#define GPIO_PORT1_B_OFFSET		0x0020		// tabla 259
#define GPIO_PORT0_DIR_OFFSET	0x2000		// tabla 261
#define GPIO_PORT1_DIR_OFFSET	0x2004		// tabla 261
#define GPIO_PORT0_SET_OFFSET	0x2200		// tabla 265
#define GPIO_PORT1_SET_OFFSET	0x2204		// tabla 265
#define GPIO_PORT0_CLR_OFFSET	0x2280		// tabla 266
#define GPIO_PORT1_CLR_OFFSET	0x2284		// tabla 266

#define PIN_INT_BASE			0x40087000		// Tabla 242, pag. 453
#define GPIO_PIN_INT			((PIN_INT_T *) PIN_INT_BASE)


#define GPIO_PORT				((GPIO_T *)	GPIO_PORT_BASE)


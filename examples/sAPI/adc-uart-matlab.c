/*============================================================================
 * Autor: Eric Pernia
 * Licencia: Beerware
 * Fecha: 2018-05-07
 *===========================================================================*/
// sale de hilo: https://groups.google.com/forum/#!topic/embebidos32/CwFgA118EIw
// sapi firmware repository: https://github.com/ciaa/firmware_v2
/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

CONSOLE_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{

   // ---------- CONFIGURACIONES ------------------------------

   // Inicializar y configurar la plataforma
   boardConfig();
   
   // Inicializar ADC
   adcConfig( ADC_ENABLE );
   
   // Inicializar UART_232 como salida Serial de consola (ac� iria el bluetooth)
   consolePrintConfigUart( UART_232, 115200 );
   
   // Variable para almacenar el valor leido del ADC CH1
   uint16_t out1 = 0;
   // Variable para almacenar el valor leido del ADC CH2
   uint16_t out2 = 0;

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {
      
      // Leo la Entrada Analogica ADC CH1
      out1 = adcRead( CH1 );
      // Leo la Entrada Analogica ADC CH2
      out2 = adcRead( CH2 );
      
      // Enviar por UART
      consolePrintInt(out1);
      consolePrintString(",");
      consolePrintlnInt(out2);
      
      // Retardo bloqueante durante 500ms
      delay( 500 );
      
      // Intercambiar el valor del led
      gpioToggle( LEDB );
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

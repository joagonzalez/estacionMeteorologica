//  Trabajo Final Electrónica Digital II - UNSAM
//  Profesor: Nicolás Alvarez
//  Alumnos: Joaquin Gonzalez, Sebastián Pedraza


//******************************************************
// Declaraciones de constantes, variables y librerias
//******************************************************
#include "funciones.h"

#define ADC_CHANNEL ADC_CH3

unsigned int adc_data;                     // variable temporal con datos obtenidos del ADC

// Toggle para TEC_[1-4]
static bool TEC_1;
static bool TEC_2;
static bool TEC_3;
static bool TEC_4;
//static bool init;

//******************************************************
// Comienzo programa principal
//******************************************************
int main(void) {
    
    //Inicializo Toggle para las Teclas
    //Entran en 1 al Handler y recorren las instrucción para cargar la señal
    //En Handler, luego, pone TEC_[i]=0 y la proxima vez que ingresen a
    //la estructura entonces removeran la señal del vector que hara que salga del DAC
    TEC_1=1;
    TEC_2=1;
    TEC_3=1;
    TEC_4=1;	
	
    // Inicio de la UART para debugging

    UART_Init();
    sprintf_mio(debug, " Inicializacion de UART\r\n");
    DEBUGSTR(debug);
   
    // Configuración de LEDs y Teclas
    //_Configuracion_IO();
    //sprintf_mio(debug, " Inicializacion de puertos GPIO\r\n");
    //DEBUGSTR(debug);

    // Inicio de ADC0
    ADC_CONFIG(ADC_CHANNEL);
    sprintf_mio(debug, " Inicializacion de puertos ADC0 CHANNEL %d\r\n", ADC_CHANNEL);
    DEBUGSTR(debug);

    // Función que tiene bucle que mantendrá corriendo el programa
    obt_datos(ADC_CHANNEL);
    
    return 0;
}

//******************************************************
// Comienzo Handlers de Interrupciones
//******************************************************

/* Funcion manejadora de la interrupcion del ADC */
void ADC0_IRQHandler(void)
{
	NVIC_DesIRQ(ADC0_IRQn);
	
	// guardamos informacion del data register del canal configurado
	adc_data = ADC0->DR[ADC_CHANNEL];

	// Se adapta adc_data a los 10 bits de informacion leidos por ADC
	adc_data = (adc_data >> 6) & 0x03FF;
	
	ADC_Interrupt_Done_Flag = 1;
	
	send_ADC_UART(adc_data);
	NVIC_EnaIRQ(ADC0_IRQn);
}

/**
//Handler TEC_1 
void GPIO0_IRQHandler(void){
	int i;

    //Debug
    sprintf_mio(debug, "GPIO0_IRQHandler - Valor de IST = 0x%X\r\n", GPIO_PIN_INT->IST);
	DEBUGSTR(debug);
	
    // Se limpia IST porque si no quedaría la interrupción activa y no ejecutaría el código del handler
	GPIO_PIN_INT->IST = (1 << 0); 

    if(TEC_1 == 1){
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
            tono_suma[i] = tono_suma[i] + tono_2_khz[i]; 
            //sprintf_mio(debug,"\r\nvalor seno: %d\n", tono_suma[i]);
	        //DEBUGSTR(debug);

      }  
      Secuencia_Tecla(TEC_1,0);
      TEC_1 = 0;
    }else{
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
          tono_suma[i] = tono_suma[i] - tono_2_khz[i]; 
      }
      Secuencia_Tecla(TEC_1,0);
      TEC_1 = 1;
      //LED_OFF(1);
    }

}

//Handler TEC_2
void GPIO1_IRQHandler(void){
	int i;

    //Debug
    sprintf_mio(debug, "GPIO1_IRQHandler - Valor de IST = 0x%X\r\n", GPIO_PIN_INT->IST);
	DEBUGSTR(debug);
	
    // Se limpia IST porque si no quedaría la interrupción activa y no ejecutaría el código del handler
	GPIO_PIN_INT->IST = (1 << 1); 

    if(TEC_2 == 1){
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
            tono_suma[i] = tono_suma[i] + tono_4_khz[i]; 
      }  
      Secuencia_Tecla(TEC_2,1);
      TEC_2 = 0;
    }else{
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
          tono_suma[i] = tono_suma[i] - tono_4_khz[i]; 
      }
      Secuencia_Tecla(TEC_2,1);
      TEC_2 = 1;
    }

}

//Handler TEC_3
void GPIO2_IRQHandler(void){
	int i;

    //Debug
    sprintf_mio(debug, "GPIO2_IRQHandler - Valor de IST = 0x%X\r\n", GPIO_PIN_INT->IST);
	DEBUGSTR(debug);
	
    // Se limpia IST porque si no quedaría la interrupción activa y no ejecutaría el código del handler
	GPIO_PIN_INT->IST = (1 << 2); 

    if(TEC_3 == 1){
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
            tono_suma[i] = tono_suma[i] + tono_8_khz[i]; 
      } 
      Secuencia_Tecla(TEC_3,2); 
      TEC_3 = 0;
    }else{
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
          tono_suma[i] = tono_suma[i] - tono_8_khz[i]; 
      }
      Secuencia_Tecla(TEC_3,2);
      TEC_3 = 1;
    }

}

//Handler TEC_4 
void GPIO3_IRQHandler(void){
	int i;

    //Debug
    sprintf_mio(debug, "GPIO3_IRQHandler - Valor de IST = 0x%X\r\n", GPIO_PIN_INT->IST);
	DEBUGSTR(debug);
	
    // Se limpia IST porque si no quedaría la interrupción activa y no ejecutaría el código del handler
	GPIO_PIN_INT->IST = (1 << 3); 

    if(TEC_4 == 1){
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
            tono_suma[i] = tono_suma[i] + tono_16_khz[i]; 
      }  
      Secuencia_Tecla(TEC_4,3);
      TEC_4 = 0;
    }else{
      for(i=0;i<WAVE_SAMPLE_NUM;i++){
          tono_suma[i] = tono_suma[i] - tono_16_khz[i]; 
      }
      Secuencia_Tecla(TEC_4,3);
      TEC_4 = 1;
    }

}**/

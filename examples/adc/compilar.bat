@echo OFF
REM **********************************************************************
REM * Script para generar el archivo de programacion del micro LPC-4337  *
REM * de la placa EDU-CIAA                                               *
REM *                                                                    *
REM * Fecha: 22/09/2016                                                  *
REM *                                                                    *
REM * Autor: Nicolas Alvarez (UNSAM)                                     *
REM *                                                                    *
REM **********************************************************************

SET NOMBRE=ADC_SysTick
SET CPATH=.\src
SET OPATH=.\obj
SET INC_LPC_PATH=-I"..\lpcopen_lpc4337\inc"
SET INC_PATH=.\inc
SET FILENAME=
REM Flag para debugging
REM SET DEBUG=-ggdb3
setlocal EnableDelayedExpansion

echo,
echo *************************************************************************************
echo *  INICIO DEL PROCESO DE COMPILACION Y PROGRAMACION DE LA PLACA EDU-CIAA (LPC4337)  *
echo *                            - %DATE% %TIME% -                         *
echo *                                                                                   *
echo *  Batch version: 1.2                                                               *
echo *  Fecha: 22/09/2016                                                                *
echo ************************************************************************************* 
echo,
echo - Borrando el contenido de los directorios bin y obj ...

del /f /q bin
del /f /q obj

for /F %%x in ('dir /B/D %CPATH%') do (
	arm-none-eabi-gcc -c -Wall %DEBUG% -fdata-sections --function-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -I%INC_PATH% %INC_LPC_PATH% -DCORE_M4 %CPATH%/%%x -o ./obj/%%x
	ren .\obj\%%x *.o
)

for /F %%x in ('dir /B/D %OPATH%') do (
	set "FILENAME=!FILENAME! %%x"
)

REM arm-none-eabi-gcc -c -Wall -ggdb3 -fdata-sections --function-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -I"./inc/"  -I"C:\CIAA\Firmware\projects\_Ejemplos\lpcopen_lpc4337\inc" -DCORE_M4 ./src/print_functions.c -o ./obj/print_functions.o

if exist obj/%NOMBRE%.o goto LINKEAR:
	echo *******************************************************************
	echo *** ERROR: No se pudo generar el archivo %NOMBRE%.o
	echo *******************************************************************
	goto end
	
:LINKEAR

echo,
echo - Generacion de los archivos objeto: OK

rem arm-none-eabi-gcc ./obj/%NOMBRE%.o -Xlinker --start-group ./liblpcopen_lpc4337.a -Xlinker --end-group -o ./bin/%NOMBRE%.axf -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -Wl,-Map="./bin/%NOMBRE%.map",-gc-sections -nostdlib -T "./ld/ciaa_lpc4337.ld"

cd obj

arm-none-eabi-gcc %FILENAME% -Xlinker --start-group ../liblpcopen_lpc4337.a -Xlinker --end-group -o ../bin/%NOMBRE%.axf -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -Wl,-Map="../bin/%NOMBRE%.map",-gc-sections -nostdlib -T "../ld/ciaa_lpc4337.ld" 
	
cd ..

if exist bin/%NOMBRE%.axf goto GENBIN:
	echo *******************************************************************
	echo *** ERROR: No se pudo generar el archivo %NOMBRE%.axf
	echo *******************************************************************
	goto end
	
:GENBIN

echo,
echo - Linkeo: OK

arm-none-eabi-objcopy -O binary ./bin/%NOMBRE%.axf ./bin/%NOMBRE%.bin

if exist bin/%NOMBRE%.bin goto OPENOCD:
	echo *******************************************************************
	echo *** ERROR: No se pudo generar el archivo %NOMBRE%.bin
	echo *******************************************************************
	goto end

:OPENOCD

echo,
echo - Generacion del archivo binario: OK
echo,

echo - Programando el micro ...
echo,

openocd -l openocd.log -f ./cfg/ciaa-nxp.cfg -c "init" -c "halt 0" -c "flash write_image erase unlock ./bin/%NOMBRE%.bin 0x1A000000 bin" -c "reset run" -c "shutdown" 1> NUL

find /I /c "Error" <openocd.log >tmp.log
set /p ERROR_C=<tmp.log

IF NOT %ERROR_C%==0	(
	echo,
	echo  *** Error en el proceso de programacion del micro!
	echo  *** Ver el archivo openocd.log
	goto end;
) ELSE (
	echo,
	echo   Proceso de programacion: OK
)

:END
del /f /q tmp.log
echo,
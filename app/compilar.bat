@echo OFF
SET BOARD=edu_cia_nxp
SET NOMBRE=estacion_meteorologica

echo,
echo ****************************************************************************************
echo   INICIO DEL PROCESO - %DATE% %TIME% - Trabajo Final Digitales II - UNSAM ECyT
echo ****************************************************************************************
echo,
echo,
echo ****************************************************************************************
echo   Alumnos: Joaquin Gonzalez (joagonzalez@gmail.com), Sebastian Pedraza (sebita202002@yahoo.com.ar)
echo ****************************************************************************************
echo,
echo - Borrando el contenido de los directorios bin y obj ...

del /f /Q bin
del /f /Q obj

arm-none-eabi-gcc -c -Wall -ggdb3 -fdata-sections --function-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -I"./inc/" -I"..\lpcopen_lpc4337\inc" -DCORE_M4 ./src/%NOMBRE%.c -o ./obj/%NOMBRE%.o

arm-none-eabi-gcc -c -Wall -ggdb3 -fdata-sections --function-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -I"./inc/" -I"..\lpcopen_lpc4337\inc" -DCORE_M4 ./src/print_functions.c -o ./obj/print_functions.o


if exist obj/%NOMBRE%.o goto LINKEAR:
	echo *******************************************************************
	echo *** ERROR: No se pudo generar el archivo %NOMBRE%.o
	echo *******************************************************************
	goto end
	
:LINKEAR

echo,
echo - Generacion de los archivos objeto: OK

arm-none-eabi-gcc ./obj/%NOMBRE%.o ./obj/print_functions.o -Xlinker --start-group ./liblpcopen_lpc4337.a -Xlinker --end-group -o ./bin/%NOMBRE%.axf -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -Wl,-Map="./bin/%NOMBRE%.map",-gc-sections -nostdlib -T "./ld/ciaa_lpc4337.ld"

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

REM find /I /c "Error" <openocd.log >tmp.log
REM set /p ERROR_C=<tmp.log

REM IF NOT %ERROR_C%==0	(
	REM echo,
	REM echo  *** Error en el proceso de programacion del micro!
	REM echo  *** Ver el archivo openocd.log
	REM goto end;
REM ) ELSE (
	REM echo,
	REM echo   Proceso de programacion: OK
REM )

:END
del /f /Q tmp.log
echo,
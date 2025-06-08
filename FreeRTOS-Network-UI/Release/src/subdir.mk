################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netmqtt.c \
../src/netntp.c \
../src/nettransport.c \
../src/netwifi.c \
../src/transport.c \
../src/uimenu.c \
../src/uiutils.c 

C_DEPS += \
./src/netmqtt.d \
./src/netntp.d \
./src/nettransport.d \
./src/netwifi.d \
./src/transport.d \
./src/uimenu.d \
./src/uiutils.d 

OBJS += \
./src/netmqtt.o \
./src/netntp.o \
./src/nettransport.o \
./src/netwifi.o \
./src/transport.o \
./src/uimenu.o \
./src/uiutils.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSIS_CORE_LPC17xx -D__LPC17XX__ -D__NEWLIB__ -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Network-UI\inc" -I"C:\Users\alext\Desktop\SE\Workspace\MQTTPacket\inc" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Drivers\inc" -I"C:\Users\alext\Desktop\SE\Workspace\DRIVERS\inc" -I"C:\Users\alext\Desktop\SE\Workspace\CMSIS_CORE_LPC17xx\inc" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\include" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\src\portable\GCC\ARM_CM3" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Template\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/netmqtt.d ./src/netmqtt.o ./src/netntp.d ./src/netntp.o ./src/nettransport.d ./src/nettransport.o ./src/netwifi.d ./src/netwifi.o ./src/transport.d ./src/transport.o ./src/uimenu.d ./src/uimenu.o ./src/uiutils.d ./src/uiutils.o

.PHONY: clean-src


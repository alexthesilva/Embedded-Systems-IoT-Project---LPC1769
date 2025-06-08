################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/info.c \
../src/rtosbh1750.c \
../src/rtosencoder.c \
../src/rtosespserial.c \
../src/rtoslcd.c \
../src/rtosled.c \
../src/rtoslock.c \
../src/rtospir.c \
../src/rtosrtc.c \
../src/rtostimeout.c 

C_DEPS += \
./src/info.d \
./src/rtosbh1750.d \
./src/rtosencoder.d \
./src/rtosespserial.d \
./src/rtoslcd.d \
./src/rtosled.d \
./src/rtoslock.d \
./src/rtospir.d \
./src/rtosrtc.d \
./src/rtostimeout.d 

OBJS += \
./src/info.o \
./src/rtosbh1750.o \
./src/rtosencoder.o \
./src/rtosespserial.o \
./src/rtoslcd.o \
./src/rtosled.o \
./src/rtoslock.o \
./src/rtospir.o \
./src/rtosrtc.o \
./src/rtostimeout.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSIS_CORE_LPC17xx -D__LPC17XX__ -D__NEWLIB__ -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Drivers\inc" -I"C:\Users\alext\Desktop\SE\Workspace\DRIVERS\inc" -I"C:\Users\alext\Desktop\SE\Workspace\CMSIS_CORE_LPC17xx\inc" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\include" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\src\portable\GCC\ARM_CM3" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Template\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/info.d ./src/info.o ./src/rtosbh1750.d ./src/rtosbh1750.o ./src/rtosencoder.d ./src/rtosencoder.o ./src/rtosespserial.d ./src/rtosespserial.o ./src/rtoslcd.d ./src/rtoslcd.o ./src/rtosled.d ./src/rtosled.o ./src/rtoslock.d ./src/rtoslock.o ./src/rtospir.d ./src/rtospir.o ./src/rtosrtc.d ./src/rtosrtc.o ./src/rtostimeout.d ./src/rtostimeout.o

.PHONY: clean-src


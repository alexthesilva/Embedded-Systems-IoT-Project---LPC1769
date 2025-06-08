################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/portable/GCC/ARM_CM3/port.c 

C_DEPS += \
./src/portable/GCC/ARM_CM3/port.d 

OBJS += \
./src/portable/GCC/ARM_CM3/port.o 


# Each subdirectory must supply rules for building sources it contributes
src/portable/GCC/ARM_CM3/%.o: ../src/portable/GCC/ARM_CM3/%.c src/portable/GCC/ARM_CM3/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__NEWLIB__ -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Template\inc" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\include" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\src\portable\GCC\ARM_CM3" -I"C:\Users\alext\Desktop\SE\Workspace\FreeRTOS-Kernel\src\portable\GCC\ARM_CM3" -I"C:\Users\alext\Desktop\SE\Workspace\CMSIS_CORE_LPC17xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-portable-2f-GCC-2f-ARM_CM3

clean-src-2f-portable-2f-GCC-2f-ARM_CM3:
	-$(RM) ./src/portable/GCC/ARM_CM3/port.d ./src/portable/GCC/ARM_CM3/port.o

.PHONY: clean-src-2f-portable-2f-GCC-2f-ARM_CM3


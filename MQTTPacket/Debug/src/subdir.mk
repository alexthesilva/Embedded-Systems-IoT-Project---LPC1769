################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MQTTConnectClient.c \
../src/MQTTConnectServer.c \
../src/MQTTDeserializePublish.c \
../src/MQTTFormat.c \
../src/MQTTPacket.c \
../src/MQTTSerializePublish.c \
../src/MQTTSubscribeClient.c \
../src/MQTTSubscribeServer.c \
../src/MQTTUnsubscribeClient.c \
../src/MQTTUnsubscribeServer.c 

C_DEPS += \
./src/MQTTConnectClient.d \
./src/MQTTConnectServer.d \
./src/MQTTDeserializePublish.d \
./src/MQTTFormat.d \
./src/MQTTPacket.d \
./src/MQTTSerializePublish.d \
./src/MQTTSubscribeClient.d \
./src/MQTTSubscribeServer.d \
./src/MQTTUnsubscribeClient.d \
./src/MQTTUnsubscribeServer.d 

OBJS += \
./src/MQTTConnectClient.o \
./src/MQTTConnectServer.o \
./src/MQTTDeserializePublish.o \
./src/MQTTFormat.o \
./src/MQTTPacket.o \
./src/MQTTSerializePublish.o \
./src/MQTTSubscribeClient.o \
./src/MQTTSubscribeServer.o \
./src/MQTTUnsubscribeClient.o \
./src/MQTTUnsubscribeServer.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__LPC17XX__ -D__NEWLIB__ -I"C:\Users\alext\Desktop\SE\Workspace\MQTTPacket\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/MQTTConnectClient.d ./src/MQTTConnectClient.o ./src/MQTTConnectServer.d ./src/MQTTConnectServer.o ./src/MQTTDeserializePublish.d ./src/MQTTDeserializePublish.o ./src/MQTTFormat.d ./src/MQTTFormat.o ./src/MQTTPacket.d ./src/MQTTPacket.o ./src/MQTTSerializePublish.d ./src/MQTTSerializePublish.o ./src/MQTTSubscribeClient.d ./src/MQTTSubscribeClient.o ./src/MQTTSubscribeServer.d ./src/MQTTSubscribeServer.o ./src/MQTTUnsubscribeClient.d ./src/MQTTUnsubscribeClient.o ./src/MQTTUnsubscribeServer.d ./src/MQTTUnsubscribeServer.o

.PHONY: clean-src


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/dma.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f1xx.c \
../Src/tim.c \
../Src/usart.c 

OBJS += \
./Src/adc.o \
./Src/dma.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f1xx.o \
./Src/tim.o \
./Src/usart.o 

C_DEPS += \
./Src/adc.d \
./Src/dma.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f1xx.d \
./Src/tim.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F103xE -I"/Users/ethanchu/Ethan/Hardware Develop/STM32_空气质量监测仪/STM32_空气质量监测仪/gitAirMonitor/AirMonitor/Inc" -I"/Users/ethanchu/Ethan/Hardware Develop/STM32_空气质量监测仪/STM32_空气质量监测仪/gitAirMonitor/AirMonitor/Drivers/STM32F1xx_HAL_Driver/Inc" -I"/Users/ethanchu/Ethan/Hardware Develop/STM32_空气质量监测仪/STM32_空气质量监测仪/gitAirMonitor/AirMonitor/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"/Users/ethanchu/Ethan/Hardware Develop/STM32_空气质量监测仪/STM32_空气质量监测仪/gitAirMonitor/AirMonitor/Drivers/CMSIS/Device/ST/STM32F1xx/Include" -I"/Users/ethanchu/Ethan/Hardware Develop/STM32_空气质量监测仪/STM32_空气质量监测仪/gitAirMonitor/AirMonitor/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



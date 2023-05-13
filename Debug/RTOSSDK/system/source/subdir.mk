################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../RTOSSDK/system/source/log.cpp \
../RTOSSDK/system/source/ret_err.cpp \
../RTOSSDK/system/source/system.cpp 

OBJS += \
./RTOSSDK/system/source/log.o \
./RTOSSDK/system/source/ret_err.o \
./RTOSSDK/system/source/system.o 

CPP_DEPS += \
./RTOSSDK/system/source/log.d \
./RTOSSDK/system/source/ret_err.d \
./RTOSSDK/system/source/system.d 


# Each subdirectory must supply rules for building sources it contributes
RTOSSDK/system/source/%.o RTOSSDK/system/source/%.su RTOSSDK/system/source/%.cyclo: ../RTOSSDK/system/source/%.cpp RTOSSDK/system/source/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -DSTM32F4_Device -D_GNU_SOURCE -DSTM32F4 -c -I../Core/Inc -I../Middlewares/Third_Party/mbedTLS/include/psa -I../RTOSSDK/config -I../RTOSSDK/peripherals -I../RTOSSDK/system -I../RTOSSDK/components -I../RTOSSDK/network -I../RTOSSDK/network/mbedtls/include -I../RTOSSDK/network/mbedtls/configs -I../RTOSSDK/protocols -I../RTOSSDK/startup -I../LWIP/App -I../LWIP/Target -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/mbedTLS/library -Ofast -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-RTOSSDK-2f-system-2f-source

clean-RTOSSDK-2f-system-2f-source:
	-$(RM) ./RTOSSDK/system/source/log.cyclo ./RTOSSDK/system/source/log.d ./RTOSSDK/system/source/log.o ./RTOSSDK/system/source/log.su ./RTOSSDK/system/source/ret_err.cyclo ./RTOSSDK/system/source/ret_err.d ./RTOSSDK/system/source/ret_err.o ./RTOSSDK/system/source/ret_err.su ./RTOSSDK/system/source/system.cyclo ./RTOSSDK/system/source/system.d ./RTOSSDK/system/source/system.o ./RTOSSDK/system/source/system.su

.PHONY: clean-RTOSSDK-2f-system-2f-source


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.cpp 

OBJS += \
./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.o 

CPP_DEPS += \
./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/RTOSSDK/components/ethphy_lan87xx/%.o Drivers/RTOSSDK/components/ethphy_lan87xx/%.su Drivers/RTOSSDK/components/ethphy_lan87xx/%.cyclo: ../Drivers/RTOSSDK/components/ethphy_lan87xx/%.cpp Drivers/RTOSSDK/components/ethphy_lan87xx/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/RTOSSDK/config -I../Drivers/RTOSSDK/peripherals -I../Drivers/RTOSSDK/components -I../Drivers/RTOSSDK/startup -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Drivers/BSP/Components/lan8742 -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-RTOSSDK-2f-components-2f-ethphy_lan87xx

clean-Drivers-2f-RTOSSDK-2f-components-2f-ethphy_lan87xx:
	-$(RM) ./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.cyclo ./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.d ./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.o ./Drivers/RTOSSDK/components/ethphy_lan87xx/ethphy_lan87xx.su

.PHONY: clean-Drivers-2f-RTOSSDK-2f-components-2f-ethphy_lan87xx

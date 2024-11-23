cmake_minimum_required(VERSION 3.22)


set(COMMON_INC 
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

set(COMMON_SRC
    ${CMAKE_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_SOURCE_DIR}/Core/Src/gpio.c
    ${CMAKE_SOURCE_DIR}/Core/Src/quadspi.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32l4xx_it.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32l4xx_hal_msp.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32l4xx.c
    ${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c
    ${CMAKE_SOURCE_DIR}/Core/Src/syscalls.c
    ${CMAKE_SOURCE_DIR}/Core/Src/w25qxx.c
)



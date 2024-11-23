cmake_minimum_required(VERSION 3.22)

# Create an executable object type
add_executable(W25Q16_STM32L4xx)

set(CMAKE_EXECUTABLE_SUFFIX ".SFL")

# Add STM32CubeMX generated sources
include(../common.cmake)

# Link directories setup
target_link_directories(W25Q16_STM32L4xx PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(W25Q16_STM32L4xx PRIVATE
    ${COMMON_SRC}
    ${CMAKE_SOURCE_DIR}/Core/Src/segger_loader.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stldr_loader.c
    # Add user sources here
)

# Add include paths
target_include_directories(W25Q16_STM32L4xx PRIVATE
    ${COMMON_INC}
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(W25Q16_STM32L4xx PRIVATE
    USE_HAL_DRIVER 
    STM32L433xx
    $<$<CONFIG:Debug>:DEBUG>
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(W25Q16_STM32L4xx
    # Add user defined libraries
)

target_link_options(W25Q16_STM32L4xx PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/segger_linker.ld)

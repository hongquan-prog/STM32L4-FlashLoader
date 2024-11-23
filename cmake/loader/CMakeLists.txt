cmake_minimum_required(VERSION 3.22)

# Create an executable object type
add_executable(Loader)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Add STM32CubeMX generated sources
include(../common.cmake)

# Link directories setup
target_link_directories(Loader PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(Loader PRIVATE
    ${COMMON_SRC}
    ${CMAKE_SOURCE_DIR}/Core/Src/stldr_loader.c
    ${CMAKE_SOURCE_DIR}/startup_stm32l433xx.s
    # Add user sources here
)

# Add include paths
target_include_directories(Loader PRIVATE
    ${COMMON_INC}
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(Loader PRIVATE
    USE_HAL_DRIVER 
    STM32L433xx
    $<$<CONFIG:Debug>:DEBUG>
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(Loader
    # Add user defined libraries
)

target_link_options(Loader PRIVATE -T${CMAKE_SOURCE_DIR}/STM32L433CCTx_FLASH.ld)

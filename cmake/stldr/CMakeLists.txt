cmake_minimum_required(VERSION 3.22)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

set(CMAKE_EXECUTABLE_SUFFIX ".stldr")

# Add STM32CubeMX generated sources
include(../common.cmake)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${COMMON_SRC}
    ${CMAKE_SOURCE_DIR}/Core/Src/stldr_loader.c
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${COMMON_INC}
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER 
    STM32L433xx
    $<$<CONFIG:Debug>:DEBUG>
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    # Add user defined libraries
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/stldr_linker.ld)

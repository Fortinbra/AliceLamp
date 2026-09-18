# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pimoroni_pga2350 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/VERSION" ALICE_LAMP_VERSION LIMIT_COUNT 1)
if(NOT ALICE_LAMP_VERSION MATCHES "^[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "VERSION must contain a MAJOR.MINOR version")
endif()

project(AliceLamp VERSION ${ALICE_LAMP_VERSION} LANGUAGES C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(AliceLamp
    src/debounced_button.cpp
    src/main.cpp
    src/ring_animation.cpp
    src/ws2812_strip.cpp)

set(ALICE_LED_COUNT 60 CACHE STRING "Number of WS2812B LEDs in the strip")
set(ALICE_LED_PIN 2 CACHE STRING "GPIO connected to the WS2812B data input")
set(ALICE_POWER_TOUCH_PIN 3 CACHE STRING "GPIO connected to the power touch sensor")
set(ALICE_CYCLE_TOUCH_PIN 4 CACHE STRING "GPIO connected to the cycle touch sensor")

target_compile_definitions(AliceLamp PRIVATE
    ALICE_LED_COUNT=${ALICE_LED_COUNT}
    ALICE_LED_PIN=${ALICE_LED_PIN}
    ALICE_POWER_TOUCH_PIN=${ALICE_POWER_TOUCH_PIN}
    ALICE_CYCLE_TOUCH_PIN=${ALICE_CYCLE_TOUCH_PIN})

pico_generate_pio_header(AliceLamp ${CMAKE_CURRENT_LIST_DIR}/pio/ws2812.pio)

pico_set_program_name(AliceLamp "AliceLamp")
pico_set_program_version(AliceLamp "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(AliceLamp 0)
pico_enable_stdio_usb(AliceLamp 1)

# Add the standard library to the build
target_link_libraries(AliceLamp
    pico_stdlib
    hardware_clocks
    hardware_pio)

target_include_directories(AliceLamp PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_options(AliceLamp PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>)

pico_add_extra_outputs(AliceLamp)


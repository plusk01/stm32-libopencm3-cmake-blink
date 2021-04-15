cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
set(CMAKE_TOOLCHAIN_FILE cmake/toolchain-arm-none-eabi-gcc.cmake)
project(blink VERSION 0.1.0 LANGUAGES CXX C ASM)

include(cmake/stm32f765.cmake)

add_executable(blink src/blink.cpp)
target_link_libraries(blink stm32f765)
set_property(TARGET blink PROPERTY CXX_STANDARD 17)
stm32_add_linker_script(blink PRIVATE src/basic.stm32.ld)
stm32_add_flash_targets(blink)
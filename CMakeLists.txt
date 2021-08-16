cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH "/mnt/d/tools/pico/pico-sdk")
set(PICO_PLATFORM "rp2040")

#add_definitions( -Dexecutable=hc_sr04 )
set( executable "hc_sr04")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialize the SDK
pico_sdk_init()

project( play_sounds C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_executable( ${executable}
        main.c
        )

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries( ${executable} pico_stdlib )

# enable usb output, disable uart output
pico_enable_stdio_usb( ${executable} 1)
pico_enable_stdio_uart( ${executable} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs( ${executable} )


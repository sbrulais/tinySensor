cmake .. -DAVR_MCU=attiny84a -DBOARD_USES_3V3_STEPUP=TRUE -DMCU_SPEED=1000000UL -DCMAKE_TOOLCHAIN_FILE=../third_party/cmake-avr/generic-gcc-avr.cmake $@

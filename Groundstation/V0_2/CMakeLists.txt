cmake_minimum_required(VERSION 3.13)
project(V0_2 C)

set(CMAKE_C_STANDARD 11)

include_directories(. /usr/local/include /usr/include)

add_executable(fanet_station
        fanet_global.c
        fanet_global.h
        fanet_mac.c
        fanet_mac.h
        fanet_mysql.c
        fanet_mysql.h
        fanet_radio.c
        fanet_radio.h
        fanet_routing.c
        fanet_routing.h
        fanet_station.c
        fanet_struct.c
        fanet_struct.h
        fanet_t0_ack.c
        fanet_t0_ack.h
        fanet_t1_tracking.c
        fanet_t1_tracking.h
        fanet_t2_name.c
        fanet_t2_name.h
        fanet_t3_messenger.c
        fanet_t3_messenger.h
        fanet_t4_service.c
        fanet_t4_service.h
        fanet_t7_tracking.c
        fanet_t7_tracking.h
        fanet_terminal.c
        fanet_terminal.h
        sx1276Regs-LoRa.h)

target_link_libraries(fanet_station m wiringPi)

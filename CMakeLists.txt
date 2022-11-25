cmake_minimum_required(VERSION 3.16)
project(bmc_tools)

set(CMAKE_CXX_STANDARD 14)

#set(COMMON_SOURCES
#	bcm_cfg.h
#        )

#
add_executable(fpga_update
	fpga_update.c
#        ${COMMON_SOURCES}
        )
target_compile_definitions(fpga_update PUBLIC)
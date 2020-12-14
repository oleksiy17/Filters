cmake_minimum_required(VERSION 3.12)

set(LIB_NAME "crossover")

project(${LIB_NAME})

add_library(${LIB_NAME} 
		../../../../chain/chain_float/${LIB_NAME}/src/crossover_control.c
		../../../../chain/chain_float/${LIB_NAME}/src/crossover_process.c)

target_include_directories(${LIB_NAME} PRIVATE
		../../../../chain/chain_float/${LIB_NAME}/inc
		../../../../lib/FixedPoint/inc
)

target_link_libraries(${LIB_NAME} PRIVATE
	FixedPoint)

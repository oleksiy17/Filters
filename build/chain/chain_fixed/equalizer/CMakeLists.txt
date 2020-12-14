cmake_minimum_required(VERSION 3.12)

set(LIB_NAME "equalizer")

project(${LIB_NAME})

add_library(${LIB_NAME} 
		../../../../chain/chain_fixed/${LIB_NAME}/src/equalizer_control.c
		../../../../chain/chain_fixed/${LIB_NAME}/src/equalizer_process.c)

target_include_directories(${LIB_NAME} PRIVATE
		../../../../chain/chain_fixed/${LIB_NAME}/inc
		../../../../chain/chain_fixed/biquad/inc
		../../../../lib/FixedPoint/inc
		
)

target_link_libraries(${LIB_NAME} PRIVATE
	FixedPoint
	biquad)

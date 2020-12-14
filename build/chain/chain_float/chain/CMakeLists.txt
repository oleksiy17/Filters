cmake_minimum_required(VERSION 3.12)

set(LIB_NAME "chain")

project(${LIB_NAME})

add_library(${LIB_NAME}
	../../../../chain/chain_float/${LIB_NAME}/src/effect_control.c
	../../../../chain/chain_float/${LIB_NAME}/src/effect_process.c)

target_include_directories(${LIB_NAME} PRIVATE 
	../../../../chain/chain_float/${LIB_NAME}/inc
	../../../../lib/FixedPoint/inc
	../../../../chain/chain_float/compressor_4ch/inc
	../../../../chain/chain_float/crossover/inc
	../../../../chain/chain_float/equalizer/inc)

target_link_libraries(${LIB_NAME} PRIVATE
	FixedPoint
	compressor_4ch
	crossover
	equalizer)
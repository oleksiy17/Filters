cmake_minimum_required(VERSION 3.12)

set(PR_NAME "Filters")

project(${PR_NAME})

add_executable(${PR_NAME}
	../../src/main.c
	../../src/fir_float.c
	../../src/iir_float.c
	../../src/fir_fixed.c
	../../src/iir_fixed.c
	../../src/float_to_pcm.c
	../../src/ring_buffer.c)

target_include_directories(${PR_NAME} PRIVATE ../../inc)
target_include_directories(${PR_NAME} PRIVATE ../../lib/FixedPoint/incFixedPoint)

target_link_libraries(${PR_NAME} PRIVATE FixedPoint)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
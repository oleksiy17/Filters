cmake_minimum_required(VERSION 3.12)

set(PR_NAME "Filters")

project(${PR_NAME})

add_executable(${PR_NAME}
	../../src/main.c
	../../src/Filter_process.c
	../../src/Filter_control.c)

target_include_directories(${PR_NAME} PRIVATE ../../inc)

#target_link_libraries(${PR_NAME} PRIVATE FixedPoint)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
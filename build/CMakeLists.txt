cmake_minimum_required(VERSION 3.12)

project(Solution)

add_subdirectory(lib/FixedPoint)
#add_subdirectory(lib/Effect/EXPANDER_float)

add_subdirectory(lib/Effect/COMPRESSOR_float)
#add_subdirectory(lib/Effect/COMPRESSOR_fixed)
#add_subdirectory(lib/Effect/GAIN_fixed)
#add_subdirectory(lib/Effect/GAIN_float)
#add_subdirectory(lib/Effect/FIR_float)
#add_subdirectory(lib/Effect/FIR_fixed)
#add_subdirectory(lib/Effect/IIR_float)
#add_subdirectory(lib/Effect/IIR_fixed)

add_subdirectory(app)


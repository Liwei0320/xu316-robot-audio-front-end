cmake_minimum_required(VERSION 3.21)

include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

project(app_sq66_beamformer)

set(APP_HW_TARGET sq66_xu316_qf60a.xn)

# lib_xua pulls in lib_mic_array and the remaining audio/USB dependencies.
set(APP_DEPENDENT_MODULES "lib_xua(5.3.0)")
set(APP_PCA_ENABLE ON)

set(APP_COMPILER_FLAGS -O3
                       -report
                       -g
                       -Wall
                       -fxscope
                       -lquadflash
                       -DXUD_CORE_CLOCK=600)

set(APP_INCLUDES src)
set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

XMOS_REGISTER_APP()

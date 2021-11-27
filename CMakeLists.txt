# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(i2c_fujitsu_fram)

FILE(GLOB app_sources src/*.c)
FILE(GLOB app_bsp src/bsp/*.c)
FILE(GLOB app_sensor src/scd41/*.c)

target_include_directories(app PRIVATE src/bsp)
target_include_directories(app PRIVATE src/scd41)
target_sources(app PRIVATE ${app_sources} ${app_bsp} ${app_sensor})

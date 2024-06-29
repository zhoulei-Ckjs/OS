cmake_minimum_required(VERSION 3.23)
project(system)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/init/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/chr_drv/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/lib/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/mm/ SRC)

file(GLOB ASM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/asm/*.asm")

message("${ASM_SOURCES}")

add_executable(${PROJECT_NAME} ${SRC} ${ASM_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
        -m32
        -masm=intel
        -fno-builtin
        -nostdinc
        -fno-pic
        -fno-pie
        -nostdlib
        -fno-stack-protector
)
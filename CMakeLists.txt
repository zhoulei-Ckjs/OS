cmake_minimum_required(VERSION 3.23)
project(system)

# 设置裸机环境编译
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_LINKER /usr/bin/ld)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/init/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/chr_drv/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/lib/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/mm/ SRC)

# .asm 编译
file(GLOB ASM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/asm/*.asm")
message("${ASM_SOURCES}")
enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")

# .c 源文件编译选项
set_source_files_properties(${SRC} PROPERTIES COMPILE_OPTIONS
        "-m32;-masm=intel;-fno-builtin;-nostdinc;-fno-pic;-fno-pie;-nostdlib;-fno-stack-protector"
)

add_executable(${PROJECT_NAME} ${SRC} ${ASM_SOURCES})

# 设置目标的链接选项
target_link_options(${PROJECT_NAME} PRIVATE -Wl,-melf_i386,-Ttext,0x1200)


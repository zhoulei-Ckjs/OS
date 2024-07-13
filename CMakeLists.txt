cmake_minimum_required(VERSION 3.23)
project(kernel)

# 设置裸机环境编译
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/init/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/chr_drv/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/lib/ SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/oskernel/mm/ SRC)

# .asm 编译
file(GLOB ASM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/asm/*.asm")
# 这个配置必须在 enable_language(ASM_NASM) 之前
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
enable_language(ASM_NASM)

# .c 源文件编译选项
set_source_files_properties(${SRC} PROPERTIES COMPILE_OPTIONS
        "-m32;-masm=intel;-fno-builtin;-nostdinc;-fno-pic;-fno-pie;-nostdlib;-fno-stack-protector"
)

# 强制设置链接器为ld，而不是gcc
set(CMAKE_C_LINK_EXECUTABLE "/usr/bin/ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
add_executable(${PROJECT_NAME} ${SRC} ${ASM_SOURCES})

# 设置目标的链接选项
target_link_options(${PROJECT_NAME} PRIVATE -m elf_i386 -Ttext 0x1200)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # 拷贝机器码
        COMMAND objcopy -O binary kernel system
        COMMAND nm kernel | sort > map
        COMMENT "拷贝机器码"
)
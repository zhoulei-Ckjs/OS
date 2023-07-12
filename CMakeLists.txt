cmake_minimum_required(VERSION 3.22)
project(kernel)

# 设置裸机环境编译
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# .asm 编译
file(GLOB ASM_SOURCES "${CMAKE_CURRENT_LIST_DIR}/oskernel/kernel/asm/*.asm")
# 这个配置必须在 enable_language(ASM_NASM) 之前
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
enable_language(ASM_NASM)

# 定义要处理的ASM文件和生成的目标文件
file(GLOB ASM_FILES "${CMAKE_CURRENT_LIST_DIR}/boot/*.asm")

foreach (ASM_FILE ${ASM_FILES})
    # 提取文件名部分，去掉 asm 后缀后存储在 FILE_NAME 中
    get_filename_component(FILE_NAME ${ASM_FILE} NAME_WE)

    # 创建自定义目标
    add_custom_target(${FILE_NAME}.o ALL
        COMMAND nasm ${ASM_FILE} -o ${FILE_NAME}.o
        DEPENDS ${ASM_FILE}
        COMMENT "Building ${FILE_NAME}.o from ${FILE_NAME}.asm"
    )
endforeach ()
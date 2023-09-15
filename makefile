BUILD:=./build

HD_IMG_NAME:= "./${BUILD}/hd.img"
SETUP_DISK_START_SECTOR=2		# setup 程序从磁盘的第 2 个扇区开始加载（从 1 开始计数，CHS 模式）
SETUP_DISK_LEN_SECTORS=2		# setup 程序在占用几个山区（1 个扇区 512 字节）
SYSTEM_START_SECTOR=3			# system.bin 在磁盘的起始扇区位置（从 0 开始计数，LBA 模式）
SYSTEM_SECTORS=34				# 内核拷贝扇区个数
LOAD_KERNEL_ADDR = 0x00			# 内核加载位置

CFLAGS:= -m32 					# 32 位的程序
CFLAGS+= -masm=intel
CFLAGS+= -fno-builtin			# 不需要 gcc 内置函数
CFLAGS+= -nostdinc				# 不需要标准头文件
CFLAGS+= -fno-pic				# 不需要位置无关的代码  position independent code
CFLAGS+= -fno-pie				# 不需要位置无关的可执行程序 position independent executable
CFLAGS+= -nostdlib				# 不需要标准库
CFLAGS+= -fno-stack-protector	# 不需要栈保护
CFLAGS:=$(strip ${CFLAGS})		# 去掉前后和中间多余的空格

INCLUDE:= -I./include			# 头文件所在目录

DEBUG:= -g

all: clean ${BUILD}/boot/boot.o ${BUILD}/boot/setup.o ${BUILD}/system.bin
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(HD_IMG_NAME)
	dd if=${BUILD}/boot/boot.o of=${HD_IMG_NAME} bs=512 seek=0 count=1 conv=notrunc
	dd if=${BUILD}/boot/setup.o of=$(HD_IMG_NAME) bs=512 seek=1 count=${SETUP_DISK_LEN_SECTORS} conv=notrunc
	dd if=${BUILD}/system.bin of=$(HD_IMG_NAME) bs=512 seek=${SYSTEM_START_SECTOR} count=${SYSTEM_SECTORS} conv=notrunc

${BUILD}/system.bin : ${BUILD}/kernel.bin
	objcopy -O binary ${BUILD}/kernel.bin ${BUILD}/system.bin
	nm ${BUILD}/kernel.bin | sort > ${BUILD}/system.map

# -Ttext 0x00	把程序的代码段（也就是 .text 段）从内存地址 0x00 开始加载。
${BUILD}/kernel.bin: ${BUILD}/boot/head.o \
	${BUILD}/init/main.o \
	${BUILD}/kernel/chr_drv/console.o \
	${BUILD}/kernel/asm/io.o \
	${BUILD}/kernel/printk.o \
	${BUILD}/kernel/vsprintf.o \
	${BUILD}/lib/string.o \
	${BUILD}/kernel/gdt.o \
	${BUILD}/kernel/idt.o \
	${BUILD}/kernel/asm/interrupt_handler.o \
	${BUILD}/kernel/chr_drv/keyboard.o \
	${BUILD}/kernel/traps.o \
	${BUILD}/kernel/exception.o \
	${BUILD}/kernel/asm/clock_handler.o \
	${BUILD}/kernel/chr_drv/clock.o \
	${BUILD}/kernel/mm/memory.o
	ld -m elf_i386 $^ -o $@ -Ttext ${LOAD_KERNEL_ADDR}

${BUILD}/lib/%.o: lib/%.c
	$(shell mkdir -p ${BUILD}/lib)
	gcc ${CFLAGS} ${INCLUDE} ${DEBUG} -c $< -o $@

${BUILD}/kernel/%.o: kernel/%.c
	$(shell mkdir -p ${BUILD}/kernel)
	gcc ${CFLAGS} ${INCLUDE} ${DEBUG} -c $< -o $@

${BUILD}/kernel/asm/%.o: kernel/asm/%.asm
	$(shell mkdir -p ${BUILD}/kernel/asm)
	nasm -f elf32 -w+error -g $< -o $@

${BUILD}/kernel/mm/%.o: kernel/mm/%.c
	$(shell mkdir -p ${BUILD}/kernel/mm)
	gcc ${CFLAGS} ${INCLUDE} ${DEBUG} -c $< -o $@

${BUILD}/kernel/chr_drv/%.o: kernel/chr_drv/%.c
	$(shell mkdir -p ${BUILD}/kernel/chr_drv)
	gcc ${CFLAGS} ${INCLUDE} ${DEBUG} -c $< -o $@

${BUILD}/init/main.o: init/main.c
	$(shell mkdir -p ${BUILD}/init)
	gcc ${CFLAGS} ${INCLUDE} ${DEBUG} -c $< -o $@

${BUILD}/boot/head.o : boot/head.asm
	nasm -f elf32 -g $< -o $@
	# -f elf32	输出格式为 elf32，生成的是 ELF 目标文件（.o），可用于 Linux 平台下与 C 一起链接成可执行程序。

${BUILD}/boot/%.o: boot/%.asm
	$(shell mkdir -p ${BUILD}/boot)
	nasm -f bin -w+error -D SETUP_DISK_START_SECTOR=${SETUP_DISK_START_SECTOR} -D SETUP_DISK_LEN_SECTORS=${SETUP_DISK_LEN_SECTORS} -D SYSTEM_START_SECTOR=${SYSTEM_START_SECTOR} -D SYSTEM_SECTORS=${SYSTEM_SECTORS} -D LOAD_KERNEL_ADDR=${LOAD_KERNEL_ADDR} $< -o $@
	# -f bin 	裸机二进制格式，编译结果是一个纯二进制文件，没有段信息、没有调试信息，也无法被链接器 ld 使用。
	# -w+error	让警告变为返回值非 0，让 Make 报错。

clean:
	$(shell rm -rf ${BUILD})
	$(shell rm -rf hd.img.lock)

bochs: all
	bochs -q -f bochsrc

qemu: all
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda ./${HD_IMG_NAME}
	# -m 32 		为虚拟机分配32MB的内存。
	# -boot c		指定虚拟机的启动顺序。-boot c 表示从虚拟硬盘（通常是/dev/hda）启动。
	# -hda ./${HD_IMG_NAME}	指定虚拟机的硬盘映像文件。-hda 表示第一个硬盘设备，././${HD_IMG_NAME} 是这个硬盘的映像文件路径。

qemug: all
	qemu-system-i386 \
    	-m 32M \
    	-boot c \
    	-hda ./${HD_IMG_NAME}\
    	-s -S
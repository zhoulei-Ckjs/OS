BUILD:=./build

HD_IMG_NAME:= "./${BUILD}/hd.img"
SYSTEM_SECTORS=9				# 内核拷贝扇区个数

all: clean ${BUILD}/boot/boot.o ${BUILD}/boot/setup.o ${BUILD}/system.bin
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(HD_IMG_NAME)
	dd if=${BUILD}/boot/boot.o of=${HD_IMG_NAME} bs=512 seek=0 count=1 conv=notrunc
	dd if=${BUILD}/boot/setup.o of=$(HD_IMG_NAME) bs=512 seek=1 count=1 conv=notrunc
	dd if=${BUILD}/system.bin of=$(HD_IMG_NAME) bs=512 seek=2 count=${SYSTEM_SECTORS} conv=notrunc

${BUILD}/system.bin : ${BUILD}/kernel.bin
	objcopy -O binary ${BUILD}/kernel.bin ${BUILD}/system.bin
	nm ${BUILD}/kernel.bin | sort > ${BUILD}/system.map

# -Ttext 0x00	把程序的代码段（也就是 .text 段）从内存地址 0x00 开始加载。
${BUILD}/kernel.bin: ${BUILD}/boot/head.o
	ld -m elf_i386 $^ -o $@ -Ttext 0x00

${BUILD}/boot/head.o : boot/head.asm
	nasm -f elf32 -g $< -o $@
	# -f elf32	输出格式为 elf32，生成的是 ELF 目标文件（.o），可用于 Linux 平台下与 C 一起链接成可执行程序。

${BUILD}/boot/%.o: boot/%.asm
	$(shell mkdir -p ${BUILD}/boot)
	nasm -f bin -D SYSTEM_SECTORS=${SYSTEM_SECTORS} $< -o $@
	# -f bin 	裸机二进制格式，编译结果是一个纯二进制文件，没有段信息、没有调试信息，也无法被链接器 ld 使用。

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
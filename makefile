BUILD:=./build

HD_IMG_NAME:= "hd.img"

all: ${BUILD}/boot/boot.o ${BUILD}/boot/setup.o
	$(shell rm -rf $(HD_IMG_NAME))
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(HD_IMG_NAME)
	dd if=${BUILD}/boot/boot.o of=${HD_IMG_NAME} bs=512 seek=0 count=1 conv=notrunc
	dd if=${BUILD}/boot/setup.o of=$(HD_IMG_NAME) bs=512 seek=1 count=1 conv=notrunc

${BUILD}/boot/%.o: boot/%.asm
	$(shell mkdir -p ${BUILD}/boot)
	nasm $< -o $@

clean:
	$(shell rm -rf ${BUILD})
	$(shell rm -rf hd.img.lock)

bochs: all
	bochs -q -f bochsrc

qemu: all
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda ./hd.img
	# -m 32 		为虚拟机分配32MB的内存。
	# -boot c		指定虚拟机的启动顺序。-boot c 表示从虚拟硬盘（通常是/dev/hda）启动。
	# -hda ./hd.img	指定虚拟机的硬盘映像文件。-hda 表示第一个硬盘设备，./hd.img 是这个硬盘的映像文件路径。
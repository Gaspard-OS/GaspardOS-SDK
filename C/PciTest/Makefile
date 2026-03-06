
SRC := mis.c PciTest.c

ARCHIVE := ABC

CFLAGS := -x c -c \
          -ffreestanding \
		  -fno-pie  -no-pie\
          -nostdlib -nodefaultlibs \
          -std=gnu11 \
          -O0 \
			-m32 \
          -g 
          
OBJETS = $(SRC:.c=.o)
LINKER   =  ../gmain_linker_x86.lld
OBJ = Pci.test
LDFLAGS := -m elf_i386   -T $(LINKER)  -nostdlib

all : clean lib_rule $(OBJETS)  archive_rule $(ARCHIVE) iso
archive_rule: 
	clang ../../archive/GaspardOSARCHIVE.c -o ../../archive/write

lib_rule:
	clang -c $(CFLAGS) ../lib/utils.c -o ../lib/utils.lib
	nasm -f elf32 ../lib/lib.asm -o ../lib/lib.lib


%.o: %.c
	clang -c $(CFLAGS) $< -o $@ 
	




$(ARCHIVE): $(OBJETS)
	ld.lld $(LDFLAGS) ../lib/utils.lib ../lib/lib.lib $(OBJETS)     -o $(OBJ)
	../../archive/write $@ $(OBJ) 

clean:
	rm -rf *.o $(ARCHIVE) ../lib/lib.lib  ../lib/utils.lib ../../archive/write YOURGaspardOS.iso

iso: 
	cp ../../iso/GaspardOS.iso YOURGaspardOS.iso
	xorriso \
		-indev  YOURGaspardOS.iso \
		-outdev YOURGaspardOS.iso \
		-map ABC /boot/ABC \
		-boot_image any keep


run: all
	qemu-system-i386 -cdrom YOURGaspardOS.iso 
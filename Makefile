arch ?= x86_64
kernel := build/doors-$(arch).bin
fat32 := build/doors-$(arch).img
loopa = /dev/loop30
loopb = /dev/loop31
linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
cfiles := src/arch/$(arch)/*.c
gcc := x86_64-elf-gcc
gcc_flags := -g -Wall -Werror -c
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
c_source_files := $(wildcard src/arch/$(arch)/*.c)
c_object_files := $(patsubst src/arch/$(arch)/%.c, \
	build/arch/$(arch)/%.o, $(c_source_files))
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

.PHONY: all clean run fat32

all: $(kernel)

loop:
	@sudo losetup -d $(loopa)
	@sudo losetup -d $(loopb)

clean:
	@sudo rm -f -r build
	@sudo rm -r /mnt/fatgrub
	@rm doors.img

run: $(clean) $(fat32)
	@qemu-system-x86_64 -s -drive file=doors.img,format=raw
	
fat32: $(fat32)

$(fat32): $(kernel) $(grub_cfg)
# Setting up the partitions
	@dd if=/dev/zero of=doors.img bs=512 count=32768
	@sudo parted doors.img mklabel msdos
	@sudo parted doors.img mkpart primary fat32 2048s 30720s
	@sudo parted doors.img set 1 boot on
	@echo $(loopa)
	@sudo losetup $(loopa) doors.img
	@echo $(loopb)
	@sudo losetup $(loopb) doors.img -o 1048576
	@sudo mkdosfs -F32 -f 2 $(loopb)



# Place Grub at start of disk
	@echo "Placing Grub"
	@sudo mkdir /mnt/fatgrub
	@sudo mount $(loopb) /mnt/fatgrub
	@sudo grub-install --root-directory=/mnt/fatgrub --no-floppy --modules="normal part_msdos ext2 multiboot" $(loopa)

# Place Kernel at 1MB offset
	@echo "Placing Kernel and related files"
	@sudo mkdir build/boot
	@sudo mkdir build/boot/grub
	@sudo cp $(kernel) build/boot
	@sudo cp $(grub_cfg) build/boot/grub
	@sudo cp -r build/boot /mnt/fatgrub

# Place grub info
# @echo 'Placing Grub'
# @

# unmount our loopbacks
	@sudo umount /mnt/fatgrub
	@sudo losetup -d $(loopa)
	@sudo losetup -d $(loopb)
	
# Building the image itself
# @echo "Bulding Image Itself"
# @mkdir -p build/fat32files/boot/grub
# @cp $(kernel) build/fat32files/boot/doors.img
# @cp $(grub_cfg) build/fat32files/boot/grub
# @grub-mkrescue -o $(fat32) build/fat32files 2> /dev/null
# @rm -r build/fat32files

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@x86_64-elf-ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)


# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@

# compile c files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.c
	@mkdir -p $(shell dirname $@)
	@$(gcc) $(gcc_flags) $< -o $@
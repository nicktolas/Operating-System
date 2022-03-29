arch ?= x86_64
kernel := build/doors-$(arch).bin
fat32 := build/os-$(arch).fat32
loopa = /dev/loop30
loopb = /dev/loop33
linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg
assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm, \
	build/arch/$(arch)/%.o, $(assembly_source_files))

.PHONY: all clean run fat32

all: $(kernel)

clean:
	@rm -r build

run: $(fat32)
	@qemu-system-x86_64 -cdrom $(fat32)
	
fat32: $(fat32)

$(fat32): $(kernel) $(grub_cfg)

	@dd if=/dev/zero of=doors.img bs=512 count=32768
	@parted doors.img mklabel msdos
	@parted doors.img mkpart primary fat32 2048s 30720s
	@parted doors.img set 1 boot on
	@echo $(loopa)
	@sudo losetup $(loopa) doors.img
	@echo $(loopb)
	@sudo losetup $(loopb) doors.img -o 1048576
	@sudo mkdosfs -F32 -f 2 $(loopb)
	@sudo mount $(loopb) /mnt/fatgrub
	@sudo grub-install --root-directory=/mnt/fatgrub --no-floppy --modules="normal part_msdos ext2 multiboot" $(loopa)
	@sudo cp -r ./doors.img/* /mnt/fatgrub

	@sudo unmount /mnt/fatgrub
	@losetup -d $(loopa)
	@losetup -d $(loopb)

	@mkdir -p build/fat32files/boot/grub
	@cp $(kernel) build/fat32files/boot/doors.bin
	@cp $(grub_cfg) build/fat32files/boot/grub
	@grub-mkrescue -o $(fat32) build/fat32files 2> /dev/null
	@rm -r build/fat32files

$(kernel): $(assembly_object_files) $(linker_script)
	@x86_64-elf-ld -n -T $(linker_script) -o $(kernel) $(assembly_object_files)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -felf64 $< -o $@
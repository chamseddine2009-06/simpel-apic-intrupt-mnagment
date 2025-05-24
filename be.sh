export PATH=$PATH:/usr/local/i386elfgcc/bin
set -e


nasm "boot.asm" -f bin -o "Binaries/boot.bin"
nasm "kernel_entry.asm" -f elf -o "Binaries/kernel_entry.o"
nasm "isr.asm" -f elf -o "Binaries/isr.o"
cd Binaries
i386-elf-gcc -lgcc -ffreestanding -m32 -g -c ../*.cpp 
cd ..
nasm "zeroes.asm" -f bin -o "Binaries/zeroes.bin"
i386-elf-ld -o "Binaries/full_kernel.bin" -Ttext 0x1000 "Binaries/kernel_entry.o" "Binaries/kernel.o" "Binaries/idt.o" "Binaries/CPU.o" "Binaries/isr.o" "Binaries/timer.o"  --oformat binary
#
#i386-elf-ld -o "Binaries/full_kernel.bin" -Ttext 0x1000 ./Binaries/*.o --oformat binary


cat "Binaries/boot.bin" "Binaries/full_kernel.bin" "Binaries/zeroes.bin"  > "Binaries/OS.bin"
#-drive format=raw,file="Binaries/OS.bin",index=0 ,if=floppy,
qemu-system-x86_64 -cpu core2duo-v1 "Binaries/OS.bin" -m 256M

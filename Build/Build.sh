# vars
cmsgl="GCC   :   '"
nmsgl="NASM  :   '"
cmsgm="' -> '"
cmsge="'"
outdir="Bin/Objs/"
cdir=""
inf=""
outf=""

# Clear the console
clear

# Setup build directory
rm -r "Bin"
mkdir "Bin"
mkdir "Bin/Objs"
mkdir "Bin/Programs"

# Boot.asm
cdir="Source/ASM/" outdir="Bin/" inf="Boot.asm" outf="Boot.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# IRQs.asm
cdir="Source/ASM/" outdir="Bin/Objs/" inf="IRQs.asm" outf="IRQs.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# ThreadSwitch.asm
cdir="Source/ASM/" outdir="Bin/Objs/" inf="ThreadSwitch.asm" outf="ThreadSwitch.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# RegisterUtil.asm
cdir="Source/ASM/" outdir="Bin/Objs/" inf="RegisterUtil.asm" outf="RegisterUtil.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# RealMode.asm
cdir="Source/ASM/" outdir="Bin/Objs/" inf="RealMode.asm" outf="RealMode.o"
nasm -felf32 "$cdir$inf" -o "$outdir$outf"
echo "$nmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"

# ----------------------------------------------- CORE --------------------------------------------------------------
cdir="Source/Kernel/Core/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- LIB --------------------------------------------------------------
cdir="Source/Lib/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- FS --------------------------------------------------------------
cdir="Source/Kernel/FS/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- SYSTEM --------------------------------------------------------------
cdir="Source/Kernel/System/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- SYSTEM/COMMANDS --------------------------------------------------------------
cdir="Source/Kernel/System/Commands/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- HAL/COMMON --------------------------------------------------------------
cdir="Source/Kernel/HAL/Common/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- HAL/DRIVERS --------------------------------------------------------------
cdir="Source/Kernel/HAL/Drivers/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- HAL/MEMORY --------------------------------------------------------------
cdir="Source/Kernel/HAL/Memory/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# ----------------------------------------------- HAL/INTERRUPTS --------------------------------------------------------------
cdir="Source/Kernel/HAL/Interrupts/"
for file in $cdir*.cpp
do
    inf="$(basename $file)"
    outf="$(echo ${inf%.*}.o)"
    i686-elf-g++ -w -IInclude -c "$cdir$inf" -o "$outdir$outf" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -fno-exceptions
    echo "$cmsgl$cdir$inf$cmsgm$outdir$outf$cmsge"
done

# Link message
linkmsg="Linking "
for file in Bin/Objs/*.o
do
    inf="$(basename $file)"
    linkmsg="$linkmsg$inf "
done
echo "$linkmsg"

# Link all files
cd 'Bin/Objs'
i686-elf-ld -T '../../Build/Linker.ld' -o '../Kernel.bin' "../Boot.o" *.o -O0
cd '../../'

# ----------------------------------------------- PROGRAMS --------------------------------------------------------------
sh Build/BuildCLib.sh
sh Build/BuildLibs.sh

# Create disk image
./fsmgr "Build/makedisk"

# Create ram disk image
./ramfsmgr "Build/makeramdisk"

# Create ISO image
mkdir -p 'Bin/isodir/boot/grub'
cp 'Bin/Kernel.bin' 'Bin/isodir/boot/kernel.bin'
cp 'ramdisk.img' 'Bin/isodir/boot/ramdisk.initrd'
cp 'Build/GrubMenu.cfg' 'Bin/isodir/boot/grub/grub.cfg'
grub-mkrescue -o 'PMOS.iso' 'Bin/isodir'
cp 'PMOS.iso' 'PMOS_FLP.img'

# Run QEMU instance of operating system
#vmplayer "../VMs/PMOS/PMOS/PMOS.vmx"
qemu-system-i386 -m 1024M -vga std -hda 'hdd.img' -cdrom 'PMOS.iso' -serial stdio -boot d -soundhw ac97 -rtc base=localtime -enable-kvm
# Libraries
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Library.o" "Disk/sys/src/Library.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Common.o" "Disk/sys/src/Common.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Time.o" "Disk/sys/src/Time.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Memory.o" "Disk/sys/src/Memory.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Graphics.o" "Disk/sys/src/gfx/Graphics.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Image.o" "Disk/sys/src/gfx/Image.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Keyboard.o" "Disk/sys/src/input/Keyboard.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Mouse.o" "Disk/sys/src/input/Mouse.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti

# Shell
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Shell.o" "Disk/sys/src/system/Shell/Main.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti
i686-elf-ld -T "Disk/sys/src/Linker.ld" -o "Disk/sys/bin/Shell.elf" "Disk/sys/bin/Shell.o" "Disk/sys/bin/Library.o" "Disk/sys/bin/Common.o" "Disk/sys/bin/Time.o" "Disk/sys/bin/Image.o" "Disk/sys/bin/Memory.o" "Disk/sys/bin/Graphics.o" "Disk/sys/bin/Keyboard.o" "Disk/sys/bin/Mouse.o" -O0

# API
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Library.o" "Disk/sys/src/api/Library.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Time.o" "Disk/sys/src/api/Time.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Memory.o" "Disk/sys/src/api/Memory.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable

# GFX
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Graphics.o" "Disk/sys/src/gfx/Graphics.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Image.o" "Disk/sys/src/gfx/Image.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable

# INPUT
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Keyboard.o" "Disk/sys/src/input/Keyboard.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Mouse.o" "Disk/sys/src/input/Mouse.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable

# UI
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Control.o" "Disk/sys/src/ui/Control.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Window.o" "Disk/sys/src/ui/Window.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/WindowManager.o" "Disk/sys/src/ui/WindowManager.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable

# Shell
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Shell.o" "Disk/sys/src/system/Shell/Main.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-ld -T "Disk/sys/src/Linker.ld" -o "Disk/sys/bin/Shell.elf" "Disk/sys/bin/Shell.o" "Disk/sys/bin/Library.o" "Disk/sys/bin/Time.o" "Disk/sys/bin/Image.o" "Disk/sys/bin/Memory.o" "Disk/sys/bin/Graphics.o" "Disk/sys/bin/Keyboard.o" "Disk/sys/bin/Mouse.o" "Disk/sys/bin/Window.o" "Disk/sys/bin/WindowManager.o" "Disk/sys/bin/Control.o" "Disk/sys/bin/ctype.o" "Disk/sys/bin/stdio.o" "Disk/sys/bin/stdlib.o" "Disk/sys/bin/string.o" -O0

# Test program
i686-elf-g++ -IDisk -c -o "Disk/sys/bin/Test.o" "Disk/sys/src/system/Terminal/Main.cpp" -nostdlib -ffreestanding -Wall -Wextra -O0 -fno-rtti -Wno-unused-parameter -Wno-unused-variable
i686-elf-ld -T "Disk/sys/src/Linker.ld" -o "Disk/sys/bin/Test.elf" "Disk/sys/bin/Test.o" "Disk/sys/bin/Library.o" "Disk/sys/bin/Memory.o" "Disk/sys/bin/ctype.o" "Disk/sys/bin/stdio.o" "Disk/sys/bin/stdlib.o" "Disk/sys/bin/string.o" -O0
# Mp3 плеер
кодек vs1011e
микроконтроллер stm32f103c8t6
экран st7735 128x160

# Возможности:
- Воспроизведение Mp3 afqkjd
- поддержка microSD (SDHC тестировано до 8 гб)
- поддуржка длиинных кириллических имён в UTF-8
- последовательный/рандомный порядок воспроизведения
- циклическое повторение одного трека


Проэкт предоставляется как есть без каких либо гарантий

Код написан на C с использованием библиотек libopencm3 и FATFS

# Instructions
 
 1. $sudo pacman -S openocd arm-none-eabi-binutils arm-none-eabi-gcc arm-none-eabi-newlib arm-none-eabi-gdb
 2. $git clone https://github.com/5881/mp3-player.git
 3. $cd mp3-player
 4. $git submodule update --init # (Only needed once)
 5. $TARGETS=stm32/f1 make -C libopencm3 # (Only needed once)
 6. $make 
 7. $make flash

Александр Белый 2019

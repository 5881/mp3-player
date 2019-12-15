/** \file printf.c
 * Simplified printf() and sprintf() implementation - prints formatted string to
 * USART (or whereever). Most common % specifiers are supported. It costs you about
 * 3k FLASH memory - without floating point support it uses only 1k ROM!
 * \author Freddie Chopin, Martin Thomas, Marten Petschke and many others
 * \date 16.2.2012
 * reduced scanf() added by Andrzej Dworzynski on the base of reduced sscanf() written by
 * some nice gay from this post: http://www.fpgarelated.com/usenet/fpga/show/61760-1.php
 * thanks to everybody:)
 * \date 12.2.2013
 */

/*****************************************************************************
 * 5 января 2019
 * Немного подправил текст библиотеки, адоптировав под libopencm3
 * Выкинул лишние файлы, уместив всё в одном.
 * 12 мая 2019
 * перепилено для работы с tft экраном
 * Shaman
 * 27 июля 2019
 * добавлена поддержка backspase '\b' в kscanf
 * Shaman
 * 17 ноября 2019
 * добавлены функции sprintf() и st_printf_at()
*****************************************************************************/ 
#ifndef STPRINTF_H
#define STPRINTF_H
int stprintf(const char *format, ...);
int stprintf_at(uint8_t x, uint8_t y,uint16_t color,uint16_t bgcolor,
						uint8_t size, const char *format, ...);
int sprintf(char *buffer, const char *format, ...);
int kscanf(const char* format, ...);
void set_scanf_mode(unsigned char);
//int fast_kscanf(const char* format, ...);


#endif

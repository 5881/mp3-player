/*Библиотека для работы с tft дисплеем 128x128 st7735
 * v 0.9
 * использована libopencm3
 * 16 mar 2019
 * Alexander Belyy
 * 
 * v0.91
 * добавлено масштабирование текста х2
 * v0.92
 * добавлен вывод побайтно как в терминале 12 мая 2019
 * v0.93
 * добавлена поддержка backspase \b 27 июля 2019
 * v0.94
 * добавлена поддержка кирилицы в cp866 11 aug 2019
 * v0.95
 * Исправлен баг с CS приводящий к артефактам
 * ошибка была в функции send_char()
 * 10 ноября 2019
 * Добавлена поддержка русской UTF8
 * v0.96
 * 17 ноября 2019
 * исправлена ошибка с печатью первого символа после очистки экрана в
 * st7735_sendchar();
 * 18 ноября 2019
 */
#ifndef ST7735_H
#define ST7735_H

#include "stdint.h"

#define HORIZONTAL 1
#define CHAR_ON_SCREEN 363
#define UTF8ASCII 1

//#define DEBUG 0

#define SPI SPI1
#define STPORT GPIOA
#define STCS GPIO1
#define DC GPIO3
#define RST GPIO2
//#define LED GPIO2
 

 
//ST7735 COMMANDS
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
//биты MADCTL
#define MY 1<<7
#define MX 1<<6
#define MV 1<<5
#define ML 1<<4
#define RGB 1<<3
#define MH 1<<2



#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF 
 

void st7735_set_printf_color(uint16_t textcolor, uint16_t bgcolor);
void st7735_sendcmd(unsigned char cmd);
void st7735_senddata(unsigned char data);
void st7735_init(void);
void st7735_wake(void);
void st7735_sleep(void);
void st7735_addrset(unsigned char sx,unsigned char sy,unsigned char ex,unsigned char ey);
void st7735_clear(uint16_t color);
void st7735_draw_s_char(unsigned char x,unsigned char y,char chr, uint16_t color, uint16_t bg_color);
void st7735_drawchar(unsigned char x,unsigned char y,char chr,uint16_t color, uint16_t bg_color);
void st7735_drawchar_x2(unsigned char x,unsigned char y,char chr,uint16_t color, uint16_t bg_color);
void st7735_drawchar_x3(unsigned char x,unsigned char y,char chr,uint16_t color, uint16_t bg_color);
void st7735_string_x3_at(unsigned char x,unsigned char y, unsigned char *chr, uint16_t color, uint16_t bg_color);
void st7735_string_x2_at(unsigned char x,unsigned char y, unsigned char *chr,	uint16_t color, uint16_t bg_color);
void st7735_string_at(unsigned char x,unsigned char y,unsigned char *chr,	uint16_t color, uint16_t bg_color);
void st7735_drawpixel(unsigned char x,unsigned char y,uint16_t Color);
void st7735_drawline(unsigned char x1, unsigned char y1,unsigned char x2,unsigned char y2, uint16_t color);
void st7735_drawcircle(unsigned char x0, unsigned char y0,unsigned char radius, uint16_t color);
void st7735_drawhl(unsigned char x,unsigned char x1,unsigned char y,uint16_t color);
void st7735_drawvl(unsigned char x,unsigned char y,unsigned char y1,uint16_t color);
void st7735_drawrectangle(unsigned char x, unsigned char y,unsigned char dx, unsigned char dy, uint16_t color);
void st7735_drawimg(uint8_t x,uint8_t y, uint8_t w,uint8_t h, uint16_t *data);
void st7735_drawgimg(uint8_t x,uint8_t y, uint8_t w,uint8_t h, uint8_t *data);
void st7735_sendchar(char ch);
void st7735_progress_bar(uint8_t y,uint8_t value, uint16_t color,uint16_t bgcolor);
void st7735_virt_sendchar(char ch);
#endif

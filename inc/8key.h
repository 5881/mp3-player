/*********************************************************************
 * Библиотека работы с клавиатурой 8 на 74HC165
 * с эмуляцией SPI
 * v 0.9
 * добавлена поддержка многократных нажатий
 * раскладка теперь хранится массивом
 * добавлена визуализация набора
 * 27 июля 2019
 *  v0.91
 * Добавлена функция f_g_key()
 * 17 ноября 2019
 *********************************************************************/
#ifndef KEY_H
#define KEY_H

#include <stdint.h>

#define HC165_PL GPIO1
#define HC165_Q7 GPIO2
#define HC165_CLK GPIO0
#define HC165_CS  GPIO10
#define HC165_PORT GPIOB
 
#define ECHO 1
 
#define echo(char) st7735_sendchar(char)
 
uint8_t read_key(void);
void key_init(void);

#endif

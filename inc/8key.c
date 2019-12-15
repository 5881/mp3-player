/*********************************************************************
 * Библиотека работы с клавиатурой 4x4 на 74HC165
 * с эмуляцией SPI
 * v 0.9
 * добавлена поддержка многократных нажатий
 * раскладка теперь хранится массивом
 * добавлена визуализация набора
 * 27 июля 2019
 * v0.91
 * Добавлена функция f_g_key()
 * 17 ноября 2019
 *********************************************************************/
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "8key.h"

 
void key_init(void){
	rcc_periph_clock_enable(HC165_PORT);
	gpio_set_mode(HC165_PORT, GPIO_MODE_OUTPUT_50_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, HC165_PL|HC165_CLK|HC165_CS);
	//Q7 это выход 74HC165
	gpio_set_mode(HC165_PORT, GPIO_MODE_INPUT,
	              GPIO_CNF_INPUT_FLOAT, HC165_Q7);
	gpio_set(HC165_PORT,HC165_CS|HC165_PL);
	gpio_clear(HC165_PORT,HC165_CLK);
	}
 
uint8_t read_key(void){
	 uint8_t data,cnt=0;
	 gpio_clear(HC165_PORT, HC165_CS);//включить тактирование
	 gpio_clear(HC165_PORT, HC165_PL);//записать значение в сдвиговый регистр
	 //delay(10);
	 gpio_set(HC165_PORT, HC165_PL);
	 for(uint8_t i=0;i<8;i++){
		data<<=1;
		if(gpio_get(HC165_PORT, HC165_Q7)) data|=1; 
		gpio_set(HC165_PORT,HC165_CLK);
		//delay(10);
		gpio_clear(HC165_PORT,HC165_CLK);
		}
	 gpio_set(HC165_PORT,HC165_CS);
	 data=~data;
	 //for(uint8_t i=0;i<8;i++){
		// if(data&1){cnt=i;break;}
		 //data>>=1;
		// }
	 return data;
	 }
 
 

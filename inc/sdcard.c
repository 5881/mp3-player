#include "sdcard.h"
#include <stdint.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>


static uint8_t send_cmd (uint8_t cmd,uint32_t arg);
static uint8_t wait_ready(void);

/*
static void set_spi_slow(){
	spi_disable(SDSPI);
	spi_set_baudrate_prescaler(SDSPI,SPI_CR1_BAUDRATE_FPCLK_DIV_128);
	spi_enable(SDSPI);
}

static void set_spi_fast(){
	spi_disable(SDSPI);
	spi_set_baudrate_prescaler(SDSPI,SPI_CR1_BAUDRATE_FPCLK_DIV_2);
	spi_enable(SDSPI);
}
*/
static uint8_t wait_ready(void){
	uint8_t res;
	uint16_t cnt=0xffff;
	spi_xfer(SDSPI, 0xff);
	do res = spi_xfer(SDSPI, 0xff); while ((res!=0xFF)&& --cnt );
	return res;
	}

static uint8_t send_cmd (uint8_t cmd,uint32_t arg){
	uint8_t n, res;
	//SDCS_DOWN();
    /* ACMD<n> is the command sequence of CMD55-CMD<n> */
	if (cmd & 0x80){
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
		}
	/* Select the card and wait for ready */
	SDCS_UP();
	SDCS_DOWN();
	if (wait_ready()!=0xFF) return 0xFF;
	/* Send command packet */
	spi_xfer(SDSPI, cmd);					/* Start + Command index */
	spi_xfer(SDSPI,(uint8_t)(arg >> 24));	/* Argument[31..24] */
	spi_xfer(SDSPI,(uint8_t)(arg >> 16));	/* Argument[23..16] */
	spi_xfer(SDSPI,(uint8_t)(arg >> 8));	/* Argument[15..8] */
	spi_xfer(SDSPI,(uint8_t)arg);			/* Argument[7..0] */
	n = 0x01;								/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;				/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;				/* Valid CRC for CMD8(0x1AA) */
	spi_xfer(SDSPI,n);
	/* Receive command response */
	if (cmd == CMD12) spi_xfer(SDSPI,0xff);	
	/* Skip a stuff byte when stop reading */
    /* Wait for a valid response in timeout of 10 attempts */
	n = 10;
	do res=spi_xfer(SDSPI,0xff); while ((res & 0x80) && --n);
	
	WSPI();
	//while (SPI_SR(SPI) & SPI_SR_BSY); //ожидание окончания передачи
	//SDCS_UP();

	return res;			           /* Return with the response value */
}

uint8_t sd_read_block(uint8_t *buf, uint32_t lba){
	uint8_t result;
	uint16_t cnt=0xffff;
	SDCS_DOWN();	
	result=send_cmd(CMD17, lba); //CMD17 даташит стр 50 и 96
	if (result){SDCS_UP(); return 5;} //Выйти, если результат не 0x00
	
	spi_xfer(SDSPI,0xff);
	cnt=0;
	do result=spi_xfer(SDSPI,0xff); while ((result!=0xFE)&&--cnt);
	if(!cnt){SDCS_UP(); return 5;}
	
	for (cnt=0;cnt<512;cnt++) *buf++=spi_xfer(SDSPI,0xff); 
	//получаем байты блока из шины в буфер
	spi_xfer(SDSPI,0xff); //Пропускаем контрольную сумму
	spi_xfer(SDSPI,0xff);
	SDCS_UP();
	spi_xfer(SDSPI,0xff);
	return 0;
	}

uint8_t sd_write_block (uint8_t *buf, uint32_t lba){
	uint8_t result;
	uint16_t cnt=0xffff;
	SDCS_DOWN();
	result=send_cmd(CMD24,lba); //CMD24 даташит стр 51 и 97-98
	if(result){SDCS_UP(); return 6;} //Выйти, если результат не 0x00
	spi_xfer(SDSPI,0xff);
	spi_xfer(SDSPI,0xfe);//Начало буфера
	for (cnt=0;cnt<512;cnt++) spi_xfer(SDSPI,buf[cnt]); //Данные
	spi_xfer(SDSPI,0xff);
	spi_xfer(SDSPI,0xff);
	result=spi_xfer(SDSPI,0xff);
	//result=wait_ready();
	if((result&0x05)!=0x05){SDCS_UP(); return 6;} 
	//spi_xfer(SDSPI,0xff);
	WSPI();
	//Выйти, если результат не 0x05 (Даташит стр 111)
	//if(wait_ready()==0xFF){SDCS_UP(); return 6;}
	SDCS_UP();
	spi_xfer(SDSPI,0xff);
	return 0;
	}


uint8_t sd_init(){
	uint8_t n, cmd, ty, ocr[4];
	uint16_t i;
	for (n = 10; n; n--) spi_xfer(SDSPI,0xff);	/* 80 dummy clocks */
	ty = 0;
	
	SDCS_DOWN();
	/* Enter Idle state */
	send_cmd(CMD0, 0);
	/* Initialization timeout of 1000 milliseconds */
    /* SDHC */
     
	if (send_cmd(CMD8, 0x1AA) == 1){
       /* Get trailing return value of R7 response */
		for (n = 0; n < 4; n++) ocr[n] = spi_xfer(SDSPI,0xff);
		/* The card can work at VDD range of 2.7-3.6V */
		if (ocr[2] == 0x01 && ocr[3] == 0xAA){
			/* Wait for leaving idle state (ACMD41 with HCS bit) */
			i=0xfff;
			while (--i && send_cmd(ACMD41, 1UL << 30));
			if (i && send_cmd(CMD58, 0) == 0){
			/* Check CCS bit in the OCR */
				for (n = 0; n < 4; n++) ocr[n] = spi_xfer(SDSPI,0xff);
				ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				//ty=66;
				}
			}
		}
	//CardType = ty;
	//release_spi();
	SDCS_UP();
	spi_xfer(SDSPI,0xff);
	WSPI();
	return ty;
	}

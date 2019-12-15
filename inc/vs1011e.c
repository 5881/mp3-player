/*Библиотека работы с vs1011e v0.00
 * 16 nov 2019
 * Shaman
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "vs1011e.h"
//нужно для отладки
#include "st_printf.h"


/* Note: code SS_VER=2 is used for both VS1002 and VS1011e */
const uint16_t chipNumber[16] = {
  1001, 1011, 1011, 1003, 1053, 1033, 1063, 1103,
  0, 0, 0, 0, 0, 0, 0, 0
};


void vs_write_sci(uint8_t addr, uint16_t data){
	while (!DREQ());
	VS_CCS_DOWN();
	spi_xfer(VS_SPI, 2);
	spi_xfer(VS_SPI, addr);
	spi_xfer(VS_SPI, (uint8_t)(data>>8));
	spi_xfer(VS_SPI, (uint8_t)(data&0xff));
	VS_CCS_UP();
	}

uint16_t vs_read_sci(uint8_t addr){
	uint16_t res;
	while (!DREQ());
	VS_CCS_DOWN();
	spi_xfer(VS_SPI, 3);
	spi_xfer(VS_SPI, addr);
	res=spi_xfer(VS_SPI, 0xff);
	res<<=8;
	res|=spi_xfer(VS_SPI, 0xff);
	VS_CCS_UP();
	return res;
	}

uint8_t vs_write_sdi(const uint8_t *data, uint8_t count){
	if (count>32) return 1;
	while (!DREQ());
	VS_DCS_DOWN();
	while(count--) spi_xfer(VS_SPI, *data++);
	VS_W_SPI();
	VS_DCS_UP();
	return 0;	
	}

void vs_vol_dec(void){
	uint16_t vol;
	//vol=vs_read_sci(SCI_VOL);
	//if(vol<=0xefef)vol+=0x0f0f; else return;
	vs_write_sci(SCI_VOL,0xbbbb);
	}
void vs_vol_inc(void){
	//uint16_t vol;
	//vol=vs_read_sci(SCI_VOL);
	//if(vol>=0x0f0f)vol-=0x0f0f; else return;
	vs_write_sci(SCI_VOL,0x2222);
	}
	
void vs_send_zero(void){
	for(uint8_t i=0;i<16;i++){
		while (!DREQ());
		VS_DCS_DOWN();
		for(uint8_t j=0;j<32;j++) spi_xfer(VS_SPI, 0x00);
		VS_W_SPI();
		VS_DCS_UP();
	}
	}
	
uint8_t vs_init(){
	//stprintf("\a start vs_init()\r\n");
	gpio_clear(VS_PORT,VS_RST);
	VS_CCS_UP();
	VS_DCS_UP();
	gpio_set(VS_PORT,VS_RST);
	//vs_write_sci(SCI_MODE, SCI_MODE_INIT_VAL);
	/* Start initialization with a dummy read, which makes sure our
     microcontoller chips selects and everything are where they
     are supposed to be and that VS10xx's SCI bus is in a known state. */
    vs_read_sci(SCI_MODE);
	/* First real operation is a software reset. After the software
     reset we know what the status of the IC is. You need, depending
     on your application, either set or not set SM_SDISHARE. See the
     Datasheet for details. */
	//vs_write_sci(SCI_MODE, SM_SDINEW|SM_TESTS|SM_RESET);
	vs_write_sci(SCI_MODE, SM_SDINEW|SM_RESET);
	
	vs_write_sci(SCI_CLOCKF, HZ_TO_SCI_CLOCKF(12000000));

  /* Set volume level at -6 dB of maximum */
	vs_write_sci(SCI_VOL, 0x3f3f);

	/* We're ready to go. */
	return 0;
	}


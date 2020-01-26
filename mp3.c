

/**********************************************************************
 * Секция include и defines
**********************************************************************/
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/cm3/scb.h>
#include "st7735_128x160.h"
#include "st_printf.h"
#include "sdcard.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "ff.h"
#include "vs1011e.h"
#include "8key.h"
//часть драйвера vs1011
#define FILE_BUFFER_SIZE 512
#define SDI_MAX_TRANSFER_SIZE 32
#define SDI_END_FILL_BYTES 512 /* Arbitrarily chosen value */
#define min(a,b) (((a)<(b))?(a):(b))

//#define SDSPIFORSAGE() {SPI_CR1(SDSPI)=(SPI_CR1(SDSPI) & 0xffc7)|1<<3;}
// Set SCLK = PCLK / 2 boudrate is bits 3-5 in SPICR1 000 is pclk/2
// 
// в libopencm3 здесь ошибка поэтому переопределим
uint8_t ulcode[3]={16,32,128}; //код разблокировки
uint8_t lock=0;
uint16_t files_count=0, track=1;
uint8_t zanuda_mode=0, rand_mode=0;
char mode[3]="  ";



static void spi2_init(void){
	//spi2 - vs1011
	/* Enable SPI1 Periph and gpio clocks */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_SPI2);
	
	/* Configure GPIOs:
	 * 
	 * SCK=PB13
	 * MOSI=PB15 
	 * MISO=PB14
	 * 
	 * for vs1011e
	 * VS_CCS PB5
	 * VS_RST PB8
	 * VS_DREQ PB7
	 * VS_DCS PB6
	 * 
	 */
	 
	//MOSI & SCK
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13|GPIO15);
    // MISO & DREQ
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,GPIO_CNF_INPUT_FLOAT,
			GPIO14|VS_DREQ);
    // CCS & VSRST & DCS    
    gpio_set_mode(VS_PORT, GPIO_MODE_OUTPUT_50_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, VS_RST|VS_CCS|VS_DCS);
	gpio_set(VS_PORT,VS_RST|VS_DCS|VS_CCS);
  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
	spi_reset(SPI2);
  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/2 of peripheral clock frequency
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
	spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_16, 
					SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1,
					SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	//spi_set_full_duplex_mode(SPI2);
  /*
   * Set NSS management to software.
   *
   * Note:
   * Setting nss high is very important, even if we are controlling 
   * the GPIO
   * ourselves this bit needs to be at least set to 1, otherwise the spi
   * peripheral will not send any data out.
   */
	spi_enable_software_slave_management(SPI2);
	spi_set_nss_high(SPI2);
  /* Enable SPI2 periph. */
	spi_enable(SPI2);
	}
	


static void spi1_init(void){
	//spi1 - display
	/* Enable SPI1 Periph and gpio clocks */
	rcc_periph_clock_enable(RCC_SPI1);
	rcc_periph_clock_enable(RCC_GPIOA);
	/* Configure GPIOs:
	 * 
	 * SCK=PA5
	 * MOSI=PA7 
	 * MISO=PA6
	 * 
	 * for st7735
	 * STCS PA1
	 * DC=PA3
	 * RST PA2
	 * 
	 * for SD card
	 * SDCS PA0
	 */
	 
	//MOSI & SCK
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5|GPIO7);
    // MISO & DREQ
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,GPIO_CNF_INPUT_FLOAT,
			GPIO6);
    // STCS & DC & RST & SDCS 
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, GPIO0|GPIO1|GPIO2|GPIO3);
	
	gpio_set(GPIOA,GPIO0|GPIO1|GPIO2|GPIO3);
  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
	spi_reset(SPI1);
  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/2 of peripheral clock frequency
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_32, 
					SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
					SPI_CR1_CPHA_CLK_TRANSITION_1,
					SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	spi_set_full_duplex_mode(SPI1);
  /*
   * Set NSS management to software.
   *
   * Note:
   * Setting nss high is very important, even if we are controlling 
   * the GPIO
   * ourselves this bit needs to be at least set to 1, otherwise the spi
   * peripheral will not send any data out.
   */
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);
  /* Enable SPI1 periph. */
	spi_enable(SPI1);
	//gpio_set(GPIOA,GPIO4);
	}
	
void led_setup(){
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
	gpio_clear(GPIOB,GPIO5);  
}

void button_setup(){
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN,
													GPIO0);
	gpio_set(GPIOB,GPIO0);  
}

void fat_tst(){
	FATFS fs;
    FRESULT res;
    res = f_mount(&fs, "", 0);
	if(res != FR_OK) {
         stprintf("f_mount() failed, res = %d\r\n", res);
        return;
		}
	uint32_t freeClust;
    FATFS* fs_ptr = &fs;
    // Warning! This fills fs.n_fatent and fs.csize!
    res = f_getfree("", &freeClust, &fs_ptr);
    if(res != FR_OK) {
        stprintf("f_getfree() failed, res = %d\r\n", res);
        return;
    }
	uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
    uint32_t freeBlocks = freeClust * fs.csize;

    stprintf("Total/Free %d/%d Mb\r\n",
						totalBlocks / 2000,freeBlocks / 2000);
    res = f_mount(NULL, "", 0);
    if(res != FR_OK) {
        stprintf("Unmount failed, res = %d\r\n", res);
        return;
		}
	}

//void name_normaliser

void unlock(uint8_t key){
	//Ждя разблокировки надо последовательно нажать три клавиши
	//указанные в массиве ulcode[3] см выше.
	static uint8_t i=0;
	//stprintf("pres key %d\r\n", ulcode[i]);
	//stprintf("key=%d,i=%d,[i]=%d\r\n",key,i,ulcode[i]);
	if(key==ulcode[i]){
		while(read_key())__asm__("nop"); i++;} else i=0;
	if(i>2){
		lock=0;
		i=0;
		stprintf("unlocked!\r\n",lock);
		return;
		}
		stprintf("pres key %d\r\n", ulcode[i]);
	}

uint8_t play_file(char *name ){
	stprintf("> %s\r\n",name);
	FIL file;
	uint8_t playBuf[512];
	uint16_t bytes_in_buffer, bytes_read, t;// How many bytes in buffer left
	uint32_t pos=0, cnt=0, fsize=0;                // File position
	uint32_t nread;
	uint16_t sr,dt,min,sec,hdat0;
	uint8_t key,bar=0,bitrate=8;
	static uint8_t vol=0x3f;
	vs_write_sci(SCI_DECODE_TIME, 0);
	if(f_open(&file, name, FA_READ)) stprintf("open file error!\r\n");
	fsize=f_size(&file);
	vs_write_sci(SCI_VOL,vol*0x101); //set default volume
	do{ f_read(&file, playBuf, FILE_BUFFER_SIZE, &bytes_read);
		uint8_t *bufP = playBuf;
		bytes_in_buffer=bytes_read;
		do{	t = min(SDI_MAX_TRANSFER_SIZE, bytes_in_buffer);
			vs_write_sdi(bufP, t);
			bufP += t;
			bytes_in_buffer -= t;
			pos += t;
			}while (bytes_in_buffer);
			
			cnt++;
			if(cnt>bitrate){
				//report
				cnt=0;
				//sr = vs_read_sci(SCI_AUDATA);
				dt = vs_read_sci(SCI_DECODE_TIME);
				hdat0=vs_read_sci(SCI_HDAT0);
				bitrate=(hdat0>>12)&0xF;
				min=dt/60;
				sec=dt%60;
				//stprintf_at(0,112,MAGENTA,BLACK,1,"=========>--------");
				bar=255*pos/fsize;
				if(zanuda_mode) st7735_progress_bar(112,bar,GREEN,BLACK);
				 else st7735_progress_bar(112,bar,MAGENTA,BLACK);
				//if(zanuda_mode)st7735_drawchar(0,112,'R',WHITE,BLACK);
				if(zanuda_mode) mode[1]='Z'; else mode[1]=' ';
				if(rand_mode) mode[0]='R'; else mode[0]='S';
				stprintf_at(0, 120,RED,BLACK,1,
					"%4d/%dK %d:%02d %s%3d/%3d",
							pos/1024,fsize/1024, min, sec, mode, track, files_count);
				
				key=read_key();
/*
 * 16 32     раскладка клавиатуры
 * 64 128
 * 1  2
 * 4  8
  */			if(!lock){	
					switch(key){
						case 2: //следующий трек
							f_close(&file);
							vs_write_sci(SCI_VOL,0xffff);//mute
							vs_send_zero();
							return 3;
							break;
						case 1: //предидущий трек
							f_close(&file);
							vs_write_sci(SCI_VOL,0xffff);//mute
							vs_send_zero();
							return 4;
							break;
						case 8://громче
							if(vol>10)vol-=10;//это не ошибка!
							vs_write_sci(SCI_VOL,vol*0x101);
							break;
						case 4://тише
							if(vol<244)vol+=10;
							vs_write_sci(SCI_VOL,vol*0x101);
							break;
						case 16://пауза
							vs_write_sci(SCI_VOL,0xffff);//mute
							vs_send_zero();
							while(read_key())__asm__("nop");
							while(!read_key())__asm__("nop");
							while(read_key())__asm__("nop");
							vs_write_sci(SCI_VOL,vol*0x101);
							break;
						case 32://повтор трека
							while(read_key())__asm__("nop");
							zanuda_mode=!zanuda_mode;
							break;
						case 64://рандом
							while(read_key())__asm__("nop");
							rand_mode=!rand_mode;
							break;
						case 128:
							lock=1;
							stprintf("keyboard are locked\r\n",lock);
							while(read_key())__asm__("nop");
							break;
						}
					
					}
					if(lock && key)unlock(key);	
					//stprintf("at play lock=%d\r\n",lock); //отладочный вывод
				}
	
	}while(bytes_read==FILE_BUFFER_SIZE);
	//stprintf("%s\r\n",temp);
	f_close(&file);
	return 0;	
	}


uint8_t ls(char *path){
	DIR dir;
	FILINFO fileInfo;
    if(f_opendir(&dir, path)) return 1;
    
    stprintf("\a%s\r\n",path);
	for(;;) {
	if (f_readdir(&dir, &fileInfo)) return 2;
		if(fileInfo.fname[0] == 0)break;
        if(fileInfo.fattrib & AM_DIR) 
				stprintf("+DIR  %s\r\n", fileInfo.fname);
        else stprintf("+ %s\r\n", fileInfo.fname);
        }
       return 0;
	}

uint8_t ismp3(char *name){
	uint16_t len;
	len=strlen(name);
	if(!strncmp(name+len-4,".mp3",3)) return 0;
		else return 1;
	}

uint16_t cnt_mp3_in_dir(char *path){
	DIR dir;
	FILINFO fileInfo;
    uint16_t count=0;
    if(f_opendir(&dir, path)) return 1;
    
    //stprintf("\a%s\r\n",path);
	for(;;) {
	if (f_readdir(&dir, &fileInfo)) return 2;
		if(fileInfo.fname[0] == 0)break;
        if(!(fileInfo.fattrib & AM_DIR)) 
				if(!ismp3(fileInfo.fname)) count++;
    }
       return count;
	}

uint8_t get_name_mp3(char *path, uint16_t n, char *name){
	DIR dir;
	FILINFO fileInfo;
    uint16_t count=0;
    if(f_opendir(&dir, path)) return 1;
    //stprintf("\a%s\r\n",path);
	while(count<n) {
	if(f_readdir(&dir, &fileInfo)) return 2;
		if(fileInfo.fname[0] == 0)return 3;
        if(!(fileInfo.fattrib & AM_DIR)) 
			if(!ismp3(fileInfo.fname)) count++;
		}
	strcpy(name,fileInfo.fname);     
    return 0;	
	}

uint8_t play_mp3_n(char *path, uint16_t n){
	char fname[257];
	uint8_t code=0;
	get_name_mp3("/",n,fname);
	code=play_file(fname);
	return code;
	}

void standby(void){
	SCB_SCR|=SCB_SCR_SLEEPDEEP;
	PWR_CR|=PWR_CR_PDDS;
	PWR_CR|=PWR_CR_CWUF;
	
	__asm__("WFI");
	}



static uint16_t get_random(void)
{
	//получение случайного числа из АЦП
	uint8_t channel = 4;
	uint16_t adc=0;
	rcc_periph_clock_enable(RCC_GPIOA);
	/* Configure GPIOs:
	 * sensor PA1
	 */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO4);
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2);
	/* Make sure the ADC doesn't run during config. */
	adc_power_off(ADC1);
	/* We configure everything for one single conversion. */
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	/* We want to read the temperature sensor, so we have to enable it. */
	//adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_1DOT5CYC);
	adc_power_on(ADC1);
	/* Wait for ADC starting up. */
	for (uint32_t i = 0; i < 800000; i++) __asm__("nop");
	//adc_reset_calibration(ADC1);
	//adc_calibrate(ADC1);
	
	
	adc_set_regular_sequence(ADC1, 1, &channel);
	adc_start_conversion_direct(ADC1);
	/* Wait for end of conversion. */
	while (!(ADC_SR(ADC1) & ADC_SR_EOC));
	adc=ADC_DR(ADC1);
	adc_power_off(ADC1);
	rcc_periph_clock_disable(RCC_ADC1);
	return adc;
}


	
void main(){
	//rcc_clock_setup_in_hse_8mhz_out_72mhz();
	//standby();
	rcc_clock_setup_in_hsi_out_24mhz();//чем медленнее тем экономичнее
	spi1_init();
	spi2_init();
		
	uint8_t status=55;
	status=sd_init();
	//если карта не стартанула нужно вызвать рест потому как 
	//повторный init не работает а сброс работает
	spi_set_baudrate_prescaler(SDSPI,SPI_CR1_BR_FPCLK_DIV_2);
	st7735_init();
	st7735_clear(BLACK);
	st7735_set_printf_color(GREEN,BLACK);
	stprintf("Display is OK!\r\n");
	if(status!=12){
		stprintf("FLASH START ERROR!\r\n");
		for(uint32_t i=0; i<0xfffff;i++) __asm__("nop");
		scb_reset_system();
		};
	//SDSPIFORSAGE();
	//spi_set_baudrate_prescaler(SDSPI,SPI_CR1_BR_FPCLK_DIV_4);
	//Внимание! SPI_CR1_BR_FPCLK_DIV_64 и SPI_CR1_BAUDRATE_FPCLK_DIV_64
	// не одно и то же!!! они сдвинуты на 3 разряда.
	
	key_init();
	//stprintf("Display is OK!\r\n");
	//while(1);
	//spi_set_baudrate_prescaler(SDSPI,SPI_CR1_BR_FPCLK_DIV_2);
	//if(status){stprintf("SD status is %d\r\n",status);fast_get_key();}
	
	vs_init();
	
	//disk init
	FATFS fs;
	uint16_t mp3_cnt=0;
	if(f_mount(&fs, "", 0)) stprintf("mount ERROR\r\n");;
	//ls("/");
	mp3_cnt=cnt_mp3_in_dir("/");
	files_count=mp3_cnt;
	stprintf("Mp3 файлов %d\r\n",mp3_cnt);
	uint16_t init_random=0;
	init_random=get_random();
	stprintf("ADC random is %d\r\n",init_random);
	srand(init_random);//инициализация ГПСЧ
	
	DIR dir;
	FILINFO fileInfo;
	if(f_opendir(&dir, "/")) stprintf("\aDISK ERROR\r\n");;
    
    //uint16_t track=1;
    uint8_t ret_code=0;
    track=rand()%mp3_cnt+1;
	while (1){
		if(mp3_cnt)ret_code=play_mp3_n("/",track);
		
		//три режима последоватьный, рандомный и зануда
		
		if(!zanuda_mode && !ret_code){ //если трек не зациклен
			if(rand_mode) track=rand()%mp3_cnt+1; //следующий случайный трек
				else if(track<mp3_cnt)track++; else track=1;
		}
		
		if(ret_code==3) if(track<mp3_cnt)track++; else track=1;//вперёд
		if(ret_code==4) if(track>1) track--; else track=mp3_cnt;//назад
				
		};
	
}


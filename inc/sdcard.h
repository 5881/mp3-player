/*****************************************************************
 * Библиотека работы с SD картой v0.00
 * 9 nov 2019 
 * Shaman
*/

#ifndef SDCARD
#define SDCARD
#include "stdint.h"

#define SDSPI SPI1
#define SDCS  GPIO0
#define SDPORT GPIOA
#define SDCS_UP() gpio_set(SDPORT, SDCS);
#define SDCS_DOWN() gpio_clear(SDPORT, SDCS);
#define SDTR(data) spi_xfer(SDPORT, (data));
#define WSPI() while(SPI_SR(SDSPI) & SPI_SR_BSY);


/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

#define CT_MMC 0x01 /* MMC ver 3 */
#define CT_SD1 0x02 /* SD ver 1 */
#define CT_SD2 0x04 /* SD ver 2 */
#define CT_SDC (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK 0x08 /* Block addressing */



uint8_t sd_init(void);
//uint8_t send_cmd (uint8_t cmd,uint32_t arg);
uint8_t sd_read_block(uint8_t *bufer, uint32_t lba);
uint8_t sd_write_block (uint8_t *buff, uint32_t lba);

#endif

/*Библиотека работы с vs1011e v0.00
 * 16 nov 2019
 * Shaman
 */



#ifndef VS1011E_H
#define VS1011E_H

#define VS_PORT GPIOB
#define VS_CCS GPIO5
#define VS_RST GPIO8
#define VS_DREQ GPIO7
#define VS_DCS GPIO6
#define VS_SPI SPI2

#define SCI_MODE_INIT_VAL SM_SDINEW
#define VS_CCS_DOWN() gpio_clear(VS_PORT, VS_CCS)
#define VS_CCS_UP() gpio_set(VS_PORT, VS_CCS)
#define VS_DCS_DOWN() gpio_clear(VS_PORT, VS_DCS)
#define VS_DCS_UP() gpio_set(VS_PORT, VS_DCS)
#define DREQ() gpio_get(VS_PORT, VS_DREQ)
#define VS_W_SPI() while(SPI_SR(VS_SPI) & SPI_SR_BSY)




void vs_send_zero(void);
void vs_write_sci(uint8_t addr, uint16_t data);
uint16_t vs_read_sci(uint8_t addr);
uint8_t vs_write_sdi(const uint8_t *data, uint8_t count);
uint8_t vs_init();
void vs_vol_inc(void);
void vs_vol_dec(void);

/* SCI registers */

#define SCI_MODE        0x00
#define SCI_STATUS      0x01
#define SCI_BASS        0x02
#define SCI_CLOCKF      0x03
#define SCI_DECODE_TIME 0x04
#define SCI_AUDATA      0x05
#define SCI_WRAM        0x06
#define SCI_WRAMADDR    0x07
#define SCI_HDAT0       0x08 /* VS1063, VS1053, VS1033, VS1003, VS1011 */
#define SCI_IN0         0x08 /* VS1103 */
#define SCI_HDAT1       0x09 /* VS1063, VS1053, VS1033, VS1003, VS1011 */
#define SCI_IN1         0x09 /* VS1103 */
#define SCI_AIADDR      0x0A
#define SCI_VOL         0x0B
#define SCI_AICTRL0     0x0C /* VS1063, VS1053, VS1033, VS1003, VS1011 */
#define SCI_MIXERVOL    0x0C /* VS1103 */
#define SCI_AICTRL1     0x0D /* VS1063, VS1053, VS1033, VS1003, VS1011 */
#define SCI_ADPCMRECCTL 0x0D /* VS1103 */
#define SCI_AICTRL2     0x0E
#define SCI_AICTRL3     0x0F



/* SCI_MODE bits */

#define SM_DIFF_B             0
#define SM_LAYER12_B          1 /* VS1063, VS1053, VS1033, VS1011 */
#define SM_RESET_B            2
#define SM_CANCEL_B           3 /* VS1063, VS1053 */
#define SM_OUTOFWAV_B         3 /* VS1033, VS1003, VS1011 */
#define SM_OUTOFMIDI_B        3 /* VS1103 */
#define SM_EARSPEAKER_LO_B    4 /* VS1053, VS1033 */
#define SM_PDOWN_B            4 /* VS1003, VS1103 */
#define SM_TESTS_B            5
#define SM_STREAM_B           6 /* VS1053, VS1033, VS1003, VS1011 */
#define SM_ICONF_B            6 /* VS1103 */
#define SM_EARSPEAKER_HI_B    7 /* VS1053, VS1033 */
#define SM_DACT_B             8
#define SM_SDIORD_B           9
#define SM_SDISHARE_B        10
#define SM_SDINEW_B          11
#define SM_ENCODE_B          12 /* VS1063 */
#define SM_ADPCM_B           12 /* VS1053, VS1033, VS1003 */
#define SM_EARSPEAKER_1103_B 12 /* VS1103 */
#define SM_ADPCM_HP_B        13 /* VS1033, VS1003 */
#define SM_LINE1_B           14 /* VS1063, VS1053 */
#define SM_LINE_IN_B         14 /* VS1033, VS1003, VS1103 */
#define SM_CLK_RANGE_B       15 /* VS1063, VS1053, VS1033 */
#define SM_ADPCM_1103_B      15 /* VS1103 */

#define SM_DIFF           (1<< 0)
#define SM_LAYER12        (1<< 1) /* VS1063, VS1053, VS1033, VS1011 */
#define SM_RECORD_PATH    (1<< 1) /* VS1103 */
#define SM_RESET          (1<< 2)
#define SM_CANCEL         (1<< 3) /* VS1063, VS1053 */
#define SM_OUTOFWAV       (1<< 3) /* VS1033, VS1003, VS1011 */
#define SM_OUTOFMIDI      (1<< 3) /* VS1103 */
#define SM_EARSPEAKER_LO  (1<< 4) /* VS1053, VS1033 */
#define SM_PDOWN          (1<< 4) /* VS1003, VS1103 */
#define SM_TESTS          (1<< 5)
#define SM_STREAM         (1<< 6) /* VS1053, VS1033, VS1003, VS1011 */
#define SM_ICONF          (1<< 6) /* VS1103 */
#define SM_EARSPEAKER_HI  (1<< 7) /* VS1053, VS1033 */
#define SM_DACT           (1<< 8)
#define SM_SDIORD         (1<< 9)
#define SM_SDISHARE       (1<<10)
#define SM_SDINEW         (1<<11)
#define SM_ENCODE         (1<<12) /* VS1063 */
#define SM_ADPCM          (1<<12) /* VS1053, VS1033, VS1003 */
#define SM_EARSPEAKER1103 (1<<12) /* VS1103 */
#define SM_ADPCM_HP       (1<<13) /* VS1033, VS1003 */
#define SM_LINE1          (1<<14) /* VS1063, VS1053 */
#define SM_LINE_IN        (1<<14) /* VS1033, VS1003, VS1103 */
#define SM_CLK_RANGE      (1<<15) /* VS1063, VS1053, VS1033 */
#define SM_ADPCM_1103     (1<<15) /* VS1103 */

#define SM_ICONF_BITS 2
#define SM_ICONF_MASK 0x00c0

#define SM_EARSPEAKER_1103_BITS 2
#define SM_EARSPEAKER_1103_MASK 0x3000


/* SCI_STATUS bits */

#define SS_REFERENCE_SEL_B  0 /* VS1063, VS1053 */
#define SS_AVOL_B           0 /* VS1033, VS1003, VS1103, VS1011 */
#define SS_AD_CLOCK_B       1 /* VS1063, VS1053 */
#define SS_APDOWN1_B        2
#define SS_APDOWN2_B        3
#define SS_VER_B            4
#define SS_VCM_DISABLE_B   10 /* VS1063, VS1053 */
#define SS_VCM_OVERLOAD_B  11 /* VS1063, VS1053 */
#define SS_SWING_B         12 /* VS1063, VS1053 */
#define SS_DO_NOT_JUMP_B   15 /* VS1063, VS1053 */

#define SS_REFERENCE_SEL (1<< 0) /* VS1063, VS1053 */
#define SS_AVOL          (1<< 0) /* VS1033, VS1003, VS1103, VS1011 */
#define SS_AD_CLOCK      (1<< 1) /* VS1063, VS1053 */
#define SS_APDOWN1       (1<< 2)
#define SS_APDOWN2       (1<< 3)
#define SS_VER           (1<< 4)
#define SS_VCM_DISABLE   (1<<10) /* VS1063, VS1053 */
#define SS_VCM_OVERLOAD  (1<<11) /* VS1063, VS1053 */
#define SS_SWING         (1<<12) /* VS1063, VS1053 */
#define SS_DO_NOT_JUMP   (1<<15) /* VS1063, VS1053 */

#define SS_SWING_BITS     3
#define SS_SWING_MASK     0x7000
#define SS_VER_BITS       4
#define SS_VER_MASK       0x00f0
#define SS_AVOL_BITS      2
#define SS_AVOL_MASK      0x0003

#define SS_VER_VS1001 0x00
#define SS_VER_VS1011 0x10
#define SS_VER_VS1002 0x20
#define SS_VER_VS1003 0x30
#define SS_VER_VS1053 0x40
#define SS_VER_VS8053 0x40
#define SS_VER_VS1033 0x50
#define SS_VER_VS1063 0x60
#define SS_VER_VS1103 0x70


/* SCI_BASS bits */

#define ST_AMPLITUDE_B 12
#define ST_FREQLIMIT_B  8
#define SB_AMPLITUDE_B  4
#define SB_FREQLIMIT_B  0

#define ST_AMPLITUDE (1<<12)
#define ST_FREQLIMIT (1<< 8)
#define SB_AMPLITUDE (1<< 4)
#define SB_FREQLIMIT (1<< 0)

#define ST_AMPLITUDE_BITS 4
#define ST_AMPLITUDE_MASK 0xf000
#define ST_FREQLIMIT_BITS 4
#define ST_FREQLIMIT_MASK 0x0f00
#define SB_AMPLITUDE_BITS 4
#define SB_AMPLITUDE_MASK 0x00f0
#define SB_FREQLIMIT_BITS 4
#define SB_FREQLIMIT_MASK 0x000f


/* SCI_CLOCKF bits */

#define SC_MULT_B 13 /* VS1063, VS1053, VS1033, VS1103, VS1003 */
#define SC_ADD_B  11 /* VS1063, VS1053, VS1033, VS1003 */
#define SC_FREQ_B  0 /* VS1063, VS1053, VS1033, VS1103, VS1003 */

#define SC_MULT (1<<13) /* VS1063, VS1053, VS1033, VS1103, VS1003 */
#define SC_ADD  (1<<11) /* VS1063, VS1053, VS1033, VS1003 */
#define SC_FREQ (1<< 0) /* VS1063, VS1053, VS1033, VS1103, VS1003 */

#define SC_MULT_BITS 3
#define SC_MULT_MASK 0xe000
#define SC_ADD_BITS 2
#define SC_ADD_MASK  0x1800
#define SC_FREQ_BITS 11
#define SC_FREQ_MASK 0x07ff

/* The following macro is for VS1011.
   The macro will automatically set the clock doubler if XTALI < 16 MHz */
#define HZ_TO_SCI_CLOCKF(hz) ((((hz)<16000000)?0x8000:0)+((hz)+1000)/2000)

/* SCI_VOL bits */

#define SV_LEFT_B  8
#define SV_RIGHT_B 0

#define SV_LEFT  (1<<8)
#define SV_RIGHT (1<<0)

#define SV_LEFT_BITS  8
#define SV_LEFT_MASK  0xFF00
#define SV_RIGHT_BITS 8
#define SV_RIGHT_MASK 0x00FF



#endif

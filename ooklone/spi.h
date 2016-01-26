/* This file was automatically generated.  Do not edit! */
#if !defined(SPI_C_H)
#define SPI_C_H 

#ifdef __cplusplus
extern "C"{
#endif


void spi_write_512(const uint8_t *s);
void spi_read_512(uint8_t *s);
void spi_read(uint8_t *s,uint8_t len);
uint8_t spi_read_uint8(void);
void spi_write(const uint8_t *s,uint8_t len);
void spi_write_uint16(uint16_t x);
void spi_write_uint8(uint8_t x);
void spi_set_sck_freq(uint8_t x);
void spi_restore_cpol(uint8_t spcr);
uint8_t spi_set_cpol(void);
void spi_setup_master(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

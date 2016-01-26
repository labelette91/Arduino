/* This file was automatically generated.  Do not edit! */

#ifdef __cplusplus
extern "C"{
#endif

uint8_t *uint32_to_string(uint32_t x);
uint8_t *uint16_to_string(uint16_t x);
uint8_t *uint8_to_string(uint8_t x);
extern uint8_t hex_buf[8];
uint8_t hex(uint8_t x);
uint8_t nibble(uint32_t x,uint8_t i);
void uart_write_rn(void);
void uart_flush_rx(void);
uint8_t uart_read_uint8(uint8_t *x);
void uart_write(const uint8_t *s,uint8_t n);
void uart_setup(void);
void set_baud_rate(long baud);

#ifdef __cplusplus
} // extern "C"
#endif

#define UART_WRITE_STRING(__s)  uart_write((uint8_t*)__s, sizeof(__s) - 1);


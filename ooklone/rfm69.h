/* This file was automatically generated.  Do not edit! */
#if !defined(RFM69_C_H)
#define RFM69_C_H 1

#ifdef __cplusplus
extern "C"{
#endif

void rfm69_set_data_low(void);
void rfm69_set_data_high(void);
void rfm69_wait_t_data(void);
uint8_t rfm69_get_data(void);
void rfm69_set_standby_mode(void);
void rfm69_set_tx_continuous_mode(void);
void rfm69_set_rx_continuous_mode(void);
void rfm69_setup(void);
void rfm69_write_rssi_threshold(uint8_t x);
uint8_t rfm69_read_irq_flags_1(void);
uint8_t rfm69_read_dio_mapping_2(void);
void rfm69_write_dio_mapping_2(uint8_t x);
uint8_t rfm69_get_rssi_avg(void);
uint8_t rfm69_get_rssi(void);
uint8_t rfm69_read_rssi_value(void);
void rfm69_write_rssi_config(uint8_t x);
uint8_t rfm69_read_rssi_config(void);
void rfm69_write_ook_fix(uint8_t x);
void rfm69_write_ook_avg(uint8_t x);
void rfm69_write_ook_peak(uint8_t x);
void rfm69_write_rx_bw(uint8_t x);
uint8_t rfm69_read_lna(void);
void rfm69_write_lna(uint8_t x);
void rfm69_write_pa_level(uint8_t x);
void rfm69_read_frf(uint32_t *x);
void rfm69_write_frf(uint32_t x);
void rfm69_write_bitrate(uint16_t x);
void rfm69_write_data_modul(uint8_t x);
uint8_t rfm69_read_op_mode(void);
void rfm69_write_op_mode(uint8_t x);
void rfm69_write_burst(uint8_t i,const uint8_t *x,uint8_t n);
void rfm69_read_burst(uint8_t i,uint8_t *x,uint8_t n);
void rfm69_write_reg(uint8_t i,uint8_t x);
uint8_t rfm69_read_reg(uint8_t i);
void rfm69_csn_wait(void);
void rfm69_csn_high(void);
void rfm69_csn_low(void);
void rfm69_csn_setup(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif



#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "./spi.h"


/* spi.csn is portb2 */
/* spi.sck is portb5 */
/* spi.mosi is portb3 */
/* spi.miso is portb4 */

#define RFM69_IO_CSN_DDR DDRB
#define RFM69_IO_CSN_PORT PORTB
#define RFM69_IO_CSN_MASK (1 << 2)

#define RFM69_IO_DIO0_DDR DDRD
#define RFM69_IO_DIO0_PIN PIND
#define RFM69_IO_DIO0_MASK (1 << 2)

/* data */
#define RFM69_IO_DIO2_DDR DDRD
#define RFM69_IO_DIO2_PIN PIND
#define RFM69_IO_DIO2_PORT PORTD
#define RFM69_IO_DIO2_MASK (1 << 3)
#define RFM69_IO_DIO2_PCICR_MASK (1 << 2)
#define RFM69_IO_DIO2_PCMSK PCMSK2

/* dclk */
#define RFM69_IO_DIO1_DDR DDRD
#define RFM69_IO_DIO1_PIN PIND
#define RFM69_IO_DIO1_PORT PORTD
#define RFM69_IO_DIO1_MASK (1 << 4)

  void rfm69_csn_setup(void)
{
  RFM69_IO_CSN_DDR |= RFM69_IO_CSN_MASK;
}

  void rfm69_csn_low(void)
{
  RFM69_IO_CSN_PORT &= ~RFM69_IO_CSN_MASK;
}

  void rfm69_csn_high(void)
{
  RFM69_IO_CSN_PORT |= RFM69_IO_CSN_MASK;
}

  void rfm69_csn_wait(void)
{
  _delay_us(1);
}

 uint8_t rfm69_read_reg(uint8_t i)
{
  /* i the register address */

  uint8_t x;

  rfm69_csn_low();
  rfm69_csn_wait();
  spi_write_uint8(i);
  x = spi_read_uint8();
  rfm69_csn_high();

  return x;
}

 void rfm69_write_reg(uint8_t i, uint8_t x)
{
  /* i the register address */
  /* x the register value */

  rfm69_csn_low();
  rfm69_csn_wait();
  spi_write_uint8((1 << 7) | i);
  spi_write_uint8(x);
  rfm69_csn_high();
}

 void rfm69_read_burst(uint8_t i, uint8_t* x, uint8_t n)
{
  /* i the register address */

  rfm69_csn_low();
  rfm69_csn_wait();
  spi_write_uint8(i);
  for (; n; --n, ++x) *x = spi_read_uint8();
  rfm69_csn_high();
}

 void rfm69_write_burst(uint8_t i, const uint8_t* x, uint8_t n)
{
  /* i the register address */

  rfm69_csn_low();
  rfm69_csn_wait();
  spi_write_uint8((1 << 7) | i);
  for (; n; --n, ++x) spi_write_uint8(*x);
  rfm69_csn_high();
}

  void rfm69_write_op_mode(uint8_t x)
{
  rfm69_write_reg(0x01, x);
}

  uint8_t rfm69_read_op_mode(void)
{
  return rfm69_read_reg(0x01);
}

  void rfm69_write_data_modul(uint8_t x)
{
  rfm69_write_reg(0x02, x);
}

  void rfm69_write_bitrate(uint16_t x)
{
  rfm69_write_reg(0x03, (x >> 8) & 0xff);
  rfm69_write_reg(0x04, (x >> 0) & 0xff);
}

  void rfm69_write_frf(uint32_t x)
{
  /* carrier frequency */
  /* note: frf lsb must be written last (note p17) */

  rfm69_write_reg(0x07, (x >> 16) & 0xff);
  rfm69_write_reg(0x08, (x >> 8) & 0xff);
  rfm69_write_reg(0x09, (x >> 0) & 0xff);
}

  void rfm69_read_frf(uint32_t* x)
{
  /* carrier frequency */

  *x = ((uint32_t)rfm69_read_reg(0x07)) << 16;
  *x |= ((uint32_t)rfm69_read_reg(0x08)) << 8;
  *x |= ((uint32_t)rfm69_read_reg(0x09)) << 0;
}

  void rfm69_write_pa_level(uint8_t x)
{
  rfm69_write_reg(0x11, x);
}

  void rfm69_write_lna(uint8_t x)
{
  rfm69_write_reg(0x18, x);
}

  uint8_t rfm69_read_lna(void)
{
  return rfm69_read_reg(0x18);
}

  void rfm69_write_rx_bw(uint8_t x)
{
  rfm69_write_reg(0x19, x);
}

  void rfm69_write_ook_peak(uint8_t x)
{
  rfm69_write_reg(0x1b, x);
}

  void rfm69_write_ook_avg(uint8_t x)
{
  rfm69_write_reg(0x1c, x);
}

  void rfm69_write_ook_fix(uint8_t x)
{
  rfm69_write_reg(0x1d, x);
}

  uint8_t rfm69_read_rssi_config(void)
{
  return rfm69_read_reg(0x23);
}

  void rfm69_write_rssi_config(uint8_t x)
{
  rfm69_write_reg(0x23, x);
}

  uint8_t rfm69_read_rssi_value(void)
{
  return rfm69_read_reg(0x24);
}

 uint8_t rfm69_get_rssi(void)
{
  rfm69_write_rssi_config(1 << 0);
  while ((rfm69_read_rssi_config() & (1 << 1)) == 0) ;
  return rfm69_read_rssi_value();
}

 uint8_t rfm69_get_rssi_avg(void)
{
  /* average the rssi level over n iterations */
  /* note: actual_rssi = - rfm69_get_rssi / 2 */

   const uint16_t n = 1000;
  uint16_t i;
  uint32_t sum = 0;
  for (i = 0; i != n; ++i) sum += rfm69_get_rssi();
  return (uint8_t)(sum / (uint32_t)n);
}

  void rfm69_write_dio_mapping_2(uint8_t x)
{
  rfm69_write_reg(0x26, x);  
}

  uint8_t rfm69_read_dio_mapping_2(void)
{
  return rfm69_read_reg(0x26);
}

  uint8_t rfm69_read_irq_flags_1(void)
{
  return rfm69_read_reg(0x27);
}

  void rfm69_write_rssi_threshold(uint8_t x)
{
  rfm69_write_reg(0x29, x);
}

 void rfm69_setup(void)
{
  uint8_t x;

  rfm69_csn_setup();
  rfm69_csn_high();

  /* dclk signal */
  RFM69_IO_DIO1_DDR |= RFM69_IO_DIO1_MASK;
  RFM69_IO_DIO1_PORT &= ~RFM69_IO_DIO1_MASK;

  /* data signal */
  RFM69_IO_DIO2_DDR &= ~RFM69_IO_DIO2_MASK;
  RFM69_IO_DIO2_PORT &= ~RFM69_IO_DIO2_MASK;

  /* put in standby mode : 01 */
  rfm69_write_op_mode(1 << 2);

  /* bitrate 03 04 */
  /* maximum ook bitrate is 32 Kbps */
  /* bitrate = fxosc / bitrate */
  /* fxosc = 32MHz */
#define RFM69_BITRATE_KBPS 32.0
  rfm69_write_bitrate((uint16_t)(32000.0 / RFM69_BITRATE_KBPS));

  /* 433.92 MHz carrier frequency */
  /* fcar = fstep * frf */
  /* fstep = fxosc / (2^19) */
  /* fxosc = 32MHz */

  /* 433.92 MHz carrier 07 08 09 */
  rfm69_write_frf((uint32_t)7109345.28);

  /* ook related values, cf. 3.4.12 */

#if 1

  /* peak mode: a one is detected when the rssi reaches */
  /* peak_thresh - 6db. the peak_thresh value is updated with */
  /* the maximum rssi value seen so far. when a zero is */
  /* detected, peak_thresh is decremented by peak_thresh_step */
  /* every peak_thresh_dec period until it reaches */
  /* fixed_thresh. */
  /* note that the period depends on the bit rate. */
  /* cf figure 12 for fixed_thresh optimzing algorithm */
  /* 1b */
  rfm69_write_ook_peak(1 << 6);
  /* 1d */
  rfm69_write_ook_fix(70);

#else

  /* fixed threshold */

  rfm69_write_ook_peak(0 << 6);
  rfm69_write_ook_fix(60);

#endif
  /* 11 */
  rfm69_write_pa_level((1 << 7) | (0x10));
  /* 18 */
  x = rfm69_read_lna();
  x = (1 << 7) | (1 << 0);
  rfm69_write_lna(x);

  /* bits<7:5>: dcc freq    010 = 4%*/
  /* bits<4:3>: rx bw mant  01 = 20 */
  /* bits<2:0>: rx bw exp   001   */
  /* 19 */
  rfm69_write_rx_bw((2 << 5) | (1 << 3) | (1 << 0));
  /*29*/
  rfm69_write_rssi_threshold(35);
}

 void rfm69_set_rx_continuous_mode(void)
{
  RFM69_IO_DIO2_DDR &= ~RFM69_IO_DIO2_MASK;
  RFM69_IO_DIO2_PORT &= ~RFM69_IO_DIO2_MASK;

  rfm69_write_data_modul((3 << 5) | (1 << 3));
  rfm69_write_op_mode((1 << 7) | (4 << 2));

  while (!(rfm69_read_irq_flags_1() & (1 << 7))) ;
}

 void rfm69_set_tx_continuous_mode(void)
{
  /* ook modulation, continuous tx */

  RFM69_IO_DIO2_DDR |= RFM69_IO_DIO2_MASK;
  RFM69_IO_DIO2_PORT &= ~RFM69_IO_DIO2_MASK;

  rfm69_write_data_modul((3 << 5) | (1 << 3));
  rfm69_write_op_mode((1 << 7) | (3 << 2));

  while (!(rfm69_read_irq_flags_1() & (1 << 7))) ;
}

 void rfm69_set_standby_mode(void)
{
  rfm69_write_op_mode((1 << 7) | (1 << 2));
  while (!(rfm69_read_irq_flags_1() & (1 << 7))) ;
}

  uint8_t rfm69_get_data(void)
{
  return RFM69_IO_DIO2_PIN & RFM69_IO_DIO2_MASK;
}

  void rfm69_wait_t_data(void)
{
  /* t_data = 250ns = 4 insn at 16MHz */

  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
  __asm__ __volatile__ ("nop");
}

  void rfm69_set_data_high(void)
{
  /* data is sampled on the rising edge of dclk/dio1 */

  RFM69_IO_DIO1_PORT &= ~RFM69_IO_DIO1_MASK;
  RFM69_IO_DIO2_PORT |= RFM69_IO_DIO2_MASK;
  rfm69_wait_t_data();
  RFM69_IO_DIO1_PORT |= RFM69_IO_DIO1_MASK;
  /* assume at least t_data from here */
}

  void rfm69_set_data_low(void)
{
  RFM69_IO_DIO1_PORT &= ~RFM69_IO_DIO1_MASK;
  RFM69_IO_DIO2_PORT &= ~RFM69_IO_DIO2_MASK;
  rfm69_wait_t_data();
  RFM69_IO_DIO1_PORT |= RFM69_IO_DIO1_MASK;
}



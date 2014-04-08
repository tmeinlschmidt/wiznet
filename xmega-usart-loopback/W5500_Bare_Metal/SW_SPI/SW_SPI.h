/*
 * SW_SPI.h
 *
 * Created: 02.02.2014 18:52:37
 *  Author: calm
 */ 
#include <avr/io.h>

#ifndef SW_SPI_H_
#define SW_SPI_H_

#define CS_WS  IOPORT_CREATE_PIN(PORTC, 1)
//#define SCL IOPORT_CREATE_PIN(PORTC, 3)
#define SCK IOPORT_CREATE_PIN(PORTC, 3)
//#define SDI IOPORT_CREATE_PIN(PORTC, 5)
//#define SDO IOPORT_CREATE_PIN(PORTC, 7)

#define MISO IOPORT_CREATE_PIN(PORTC, 5)
#define MOSI IOPORT_CREATE_PIN(PORTC, 7)

/*
#define CF1 IOPORT_CREATE_PIN(PORTD, 3)
#define CF2 IOPORT_CREATE_PIN(PORTD, 2)
#define CF3 IOPORT_CREATE_PIN(PORTD, 1)
#define CF4 IOPORT_CREATE_PIN(PORTD, 0)

#define LED IOPORT_CREATE_PIN(PORTA, 5)
*/
void  chip_deselect_sw(void);
void  chip_select_sw(void);
void  spi_write_sw(uint8_t wb);
uint8_t spi_read_sw(void);
void init_ports(void);
void spi_init_sw(void);


#endif /* SW_SPI_H_ */
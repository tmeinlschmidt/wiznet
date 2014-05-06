/*
 * TCP_2_USART.h
 *
 * Created: 23.03.2014 22:12:47
 *  Author: calm
 */ 


#ifndef TCP_2_USART_H_
#define TCP_2_USART_H_

//Common settings for all possibility
#define USART_SERIAL                     &USARTD0
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
//#define USART_SERIAL_STOP_BIT            false
//#define USART_SERIAL_BAUDRATE            115200

#define TX IOPORT_CREATE_PIN(PORTD, 7)
#define RX IOPORT_CREATE_PIN(PORTD, 6)
void usart_setup(uint32_t Baudrate, bool Stopbit);
void LAN2Serial(uint8_t config);


#endif /* TCP_2_USART_H_ */
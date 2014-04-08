/*
 * USART_Sample.h
 *
 * Created: 07.04.2014 22:26:51
 *  Author: calm
 */ 

#define USART_SERIAL                     &USARTD0
#define USART_SERIAL_BAUDRATE            9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

#define TX IOPORT_CREATE_PIN(PORTD, 7)
#define RX IOPORT_CREATE_PIN(PORTD, 6)

void usart_setup(void);
void usart_loopback(void);
void usart_sample(void);
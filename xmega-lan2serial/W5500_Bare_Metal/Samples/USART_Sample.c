/*
 * USART_Sample.c
 *
 * Created: 07.04.2014 22:26:37
 *  Author: calm
 */ 

#include <asf.h>
#include "USART_Sample.h"

ISR(USARTD0_RXC_vect)
{
	//Get the char out of the Buffer
	//And Put it back to the USART
	uint8_t recived = usart_getchar(&USARTD0);
	usart_putchar(&USARTD0,recived);
	
}

void usart_setup()
{
	//Not nice but just a sample
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	//Setup Clock
	sysclk_enable_module(SYSCLK_PORT_D, PR_USART0_bm);
	//Setup USARTD0 as RS232
	usart_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);
	
	//Remap Pins to the correct place
	PORTD.REMAP|=(0x10);
	//Set IOs to IN and OUT
	ioport_set_pin_dir(TX,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(RX,IOPORT_DIR_INPUT);
	
	//Init PMIC
	pmic_init();	
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
	//Set USARTD RX ISR to level high
	usart_set_rx_interrupt_level(&USARTD0,USART_INT_LVL_HI);
	//Enable HIGH LEVEL ISR
	pmic_enable_level(PMIC_LVL_HIGH);
	//Enable Global ISR
	cpu_irq_enable();
}


void usart_loopback(void)
{
	
	while(1==1)
	{
		
	}
}

void usart_sample()
{
	usart_setup();
	usart_loopback();
}
/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "../Ethernet/Eth.h"
#include "../Ethernet/socket.h"
#include "../Ethernet/wizchip_conf.h"
//#include "../Samples/TCP_IP_Loopback.h"
//#include "../Samples/USART_Sample.h"
#include "../Samples/TCP_2_USART.h"

#define CF1 IOPORT_CREATE_PIN(PORTD, 3)
#define CF2 IOPORT_CREATE_PIN(PORTD, 2)
#define CF3 IOPORT_CREATE_PIN(PORTD, 1)
#define CF4 IOPORT_CREATE_PIN(PORTD, 0)
#define LED IOPORT_CREATE_PIN(PORTA, 5)


//To Capture unwanted ISRs
ISR(BADISR_vect)
{
	
}
void SW_Reset(void);

void SW_Reset(void)
{
	CCP = CCP_IOREG_gc;
	RST_CTRL =  RST_SWRST_bm;
}

int main (void)
{
	uint8_t config=0;
	wiz_NetInfo NetworkConfig = {
		.mac = {0x00, 0x08, 0xdc,0x00, 0x00, 0x01},
		.ip = {192, 168, 1, 123},
		.sn = {255,255,255,0},
		.gw = {192, 168, 1, 1},
		.dns = {0,0,0,0},
	.dhcp = NETINFO_STATIC };
	
	
	board_init();
	ioport_init();
	sysclk_init();
	

	
	
	cli();
	sleepmgr_init();

	ioport_set_pin_dir(CF1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CF2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CF3, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(CF4, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(CF1, IOPORT_MODE_PULLUP);
	ioport_set_pin_mode(CF2, IOPORT_MODE_PULLUP);
	ioport_set_pin_mode(CF3, IOPORT_MODE_PULLUP);
	ioport_set_pin_mode(CF4, IOPORT_MODE_PULLUP);
	
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_high(LED);
	
	//Read Input
	if(ioport_get_pin_level(CF1)==0)
	{
		config|=0x01;
	}
	
	if(ioport_get_pin_level(CF2)==0)
	{
		config|=0x02;
	}
	
	if(ioport_get_pin_level(CF3)==0)
	{
		config|=0x04;
	}
	
	if(ioport_get_pin_level(CF4)==0)
	{
		config|=0x08;
	}
		
	delay_ms(100);	

 	ioport_set_pin_high(LED);
		
	w55500_init(&NetworkConfig);
	
	while(1==1)
	{
		//Here will be some Samples, look inside the Samples for more Infos
		
		//-----------------------------------------------------------------
		//
		// This is the LoopBack Demo for TCP/IP
		//TCP_IP_Loopback();
		//usart_sample();
		LAN2Serial(config);
		
	}
}



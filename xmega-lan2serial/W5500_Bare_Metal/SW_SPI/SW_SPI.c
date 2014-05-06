/*
 * SW_SPI.c
 *
 * Created: 02.02.2014 18:52:21
 *  Author: calm
 */ 

#include "SW_SPI.h"
#include <asf.h>

//Set Chipselect appropriate for the W5500
void  chip_deselect_sw(void)
{
	ioport_set_pin_high(CS_WS); //Pin is defined in SW_SPI.h
}

//Set Chipselect appropriate for the W5500 
void  chip_select_sw(void)
{
	ioport_set_pin_low(CS_WS);
}

//This simpli writes Data to the Chip but doesn't return any Data that is shifted back
void  spi_write_sw(uint8_t wb)
{

	ioport_set_pin_low(MOSI);			//Set MOSI to a defined level
	ioport_set_pin_low(SCK);			//Set Clock to a defined level
	for(uint8_t i=0;i<8;i++)			//8 Bit Transfer
	{
		if((wb&0x80)>0)					//We will shift MSB to LSB out, so lets see if MSB is set
		{
			
			ioport_set_pin_high(MOSI);	//Okay MSB is set lets put MOSI accordingly
		}
		
		
		ioport_set_pin_high(SCK);		//Toggle Clock, first High
		ioport_set_pin_low(SCK);		//then Low
		
		ioport_set_pin_low(MOSI);		//Set MOSI to a given Level
		wb=(wb<<1);						//Bitshift for the Datatransfer
	}
	
	ioport_set_pin_low(MOSI);			//Set MOSI to a defined level (idle)
	ioport_set_pin_low(SCK);			//Set SCK to a defined level (idle)
	
}

//Ths reads Data and transmitts simply 0x00 to the device, data will be read back and returnd as 8-Bit Transfer
uint8_t spi_read_sw()
{
	uint8_t spi_reg=0;
	ioport_set_pin_low(MOSI);
	ioport_set_pin_low(SCK);
	for(uint8_t i=0;i<8;i++)
	{
		
		
		ioport_set_pin_high(SCK);
		spi_reg=(spi_reg<<1);
		
		
		if(ioport_get_pin_level(MISO)>0)
		{
			spi_reg|=1;
		}
		
		
		ioport_set_pin_low(MOSI);
		ioport_set_pin_low(SCK);
		
	}	
	ioport_set_pin_low(MOSI);
	ioport_set_pin_low(SCK);
	return spi_reg;
}

void init_ports()
{

	ioport_set_pin_high(CS_WS);
	ioport_set_pin_dir(CS_WS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SCK, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(MOSI, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(MISO, IOPORT_DIR_INPUT);
}

void spi_init_sw(void)
{
	init_ports();
	chip_deselect_sw();
}
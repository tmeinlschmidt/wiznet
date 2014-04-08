/*
 * Eth.c
 *
 * Created: 02.02.2014 18:28:31
 *  Author: calm
 */ 

#include "Eth.h"
#include "./W5500/w5500.h"
#include "../SW_SPI/SW_SPI.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <asf.h>

void enter_critical(void)
{
	//What ever is required to enter your critical section do it here
}

void exit_critical(void)
{
	//What ever is required to leave your critical section do it here
}


void w5500_init_io(void)
{
		ioport_set_pin_dir(RSTN, IOPORT_DIR_OUTPUT);	//Set Reset-Pin in right mode
		ioport_set_pin_dir(INTN, IOPORT_DIR_INPUT);		//Set Int-Pin in right mode
		ioport_set_pin_high(RSTN);						//Set Reset-Pin High and get W5500 out of Reset
}

void w5500_hw_reset(void)
{	
		ioport_set_pin_low(RSTN);			//Set Reset Low
		_delay_ms(3);						//Wait a bit to let Chip enter Reset
		ioport_set_pin_high(RSTN);			//Release Reset
}



InitResult_t w55500_init(wiz_NetInfo* NetworkConfig)
{
	uint8_t tmp;
	uint8_t memsize[2][8] = {
							 {2,2,2,2,2,2,2,2} /* Configure RX Socket Size in kByte*/
							,{2,2,2,2,2,2,2,2} /* Configure TX Socket Size in kByte*/
							}; 
	
	w5500_init_io();		//Set the additional IOs to the correct value and direction
	
	spi_init_sw();			//Init the Software SPI Driver
	/* Critical section callback - No use in this example */
	//reg_wizchip_cris_cbfunc(0, 0);
	/* Chip selection call back */
	#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(chip_select_sw, chip_deselect_sw);		//Register Functionpointer for Chipselect in Driver
	#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(wizchip_select_sw, wizchip_select_sw);	//Register Functionpointer for Chipselect in Driver
	#else
	#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
	#error "Unknown _WIZCHIP_IO_MODE_"
	#else
	reg_wizchip_cs_cbfunc(chip_select_sw, chip_deselect_sw);		//Register Functionpointer for Chipselect in Driver
	#endif
	#endif
	/* SPI Read & Write callback function */
	reg_wizchip_spi_cbfunc(spi_read_sw, spi_write_sw);				//Register Functionpointer for Datatransfers in Driver
	/* Enter- und Exit-Critical, cli() and sei()...*/
	//reg_wizchip_cris_cbfunc(enter_critical,exit_critical);		///Use with care !
	////////////////////////////////////////////////////////////////////////

	
	/* WIZCHIP SOCKET Buffer initialize */
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		return InitError;	//Init went wrong
	}
	
	/* PHY link status check */
	if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
	{
		return PhyError;	//Phy in suspect state
	}
	
	//This is wehre the "Black Magic" happens
	//
	ctlnetwork(CN_SET_NETINFO,NetworkConfig);	//Write Config in Chip
	ctlnetwork(CN_GET_NETINFO,NetworkConfig);	//Read Config Back
	return Okay;								//Return Okay
	//Black Magic done :-), as you given a pointer to your Network-Config, you'll get the current config back from the chip
}
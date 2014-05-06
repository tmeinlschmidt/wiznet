/*
 * Eth.h
 *
 * Created: 02.02.2014 18:28:58
 *  Author: calm
 */ 


#ifndef ETH_H_
#define ETH_H_

#include <avr/io.h>
#include "../SW_SPI/SW_SPI.h"
#include "./W5500/w5500.h"

typedef enum {Okay,PhyError,InitError} InitResult_t;

#define INTN IOPORT_CREATE_PIN(PORTC, 4)
#define RSTN IOPORT_CREATE_PIN(PORTC, 6)


/////////////////////////////////////////
// SOCKET NUMBER DEFINION for Examples //
/////////////////////////////////////////

void w5500_init_io(void);
void w5500_hw_reset(void);
void enter_critical(void);
void exit_critical(void);
InitResult_t w55500_init(wiz_NetInfo*);


#endif /* ETH_H_ */
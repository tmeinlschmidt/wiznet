/*
 * TCP_2_USART.c
 *
 * Created: 23.03.2014 22:12:56
 *  Author: calm
 */ 
#include "asf.h"
#include "../Ethernet/socket.h"
#include "TCP_2_USART.h"

#define L2S_Socket	1
#define Port 8080
#define BUF_SIZE_T2U 128
uint8_t Buffer[BUF_SIZE_T2U]={0,};
	
int32_t tcp_2_usart(uint8_t sn, uint8_t* buf, uint16_t port);
uint32_t usart_2_tcp(uint8_t sn);

#define FIFO_BUFFER_LENGTH  128
uint8_t fifo_buffer[FIFO_BUFFER_LENGTH]; //FiFo Buffer
fifo_desc_t fifo_desc; 

uint8_t fifo_buffer_usart_tx[FIFO_BUFFER_LENGTH]; //FiFo Buffer
fifo_desc_t fifo_desc_usart_tx;


void usart_setup(uint32_t config, bool Stopbit);


ISR(USARTD0_RXC_vect)
{
	//Get the char out of the Buffer
	//And Put it to the FiFo
	volatile uint8_t recived = usart_getchar(&USARTD0);
	fifo_push_uint8(&fifo_desc,recived);
}

ISR(USARTD0_DRE_vect)
{
	//The DATAREGISTER is empty, what to do?
	uint8_t Data=0;
	if(fifo_pull_uint8(&fifo_desc_usart_tx,&Data)==FIFO_OK)
	{
		usart_putchar(&USARTD0,Data);
	}
	else
	{
		//Fifo Empty or broken
		usart_set_dre_interrupt_level(&USARTD0,USART_INT_LVL_OFF);
	}
	
}


void usart_setup(uint32_t Baudrate, bool Stopbit)
{
	//Not nice but just a sample
	//115200Baud 8Bit No Parity Two Stopbit
	usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 0 ,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = false
	};
	USART_SERIAL_OPTIONS.baudrate=Baudrate;
	USART_SERIAL_OPTIONS.stopbits=Stopbit;
	
	
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
	//Ste Scheduling to round-robbin to get every interrupt served once a while
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
	//Set USARTD RX ISR to level high
	usart_set_rx_interrupt_level(&USARTD0,USART_INT_LVL_HI);
	//Enable HIGH LEVEL ISR
	pmic_enable_level(PMIC_LVL_HIGH);
	
	//Set USARTD DRE ISR to level off...
	usart_set_dre_interrupt_level(&USARTD0,USART_INT_LVL_OFF);
	//Enable HIGH LEVEL ISR
	pmic_enable_level(PMIC_LVL_HIGH);
	
	//Enable Global ISR
	cpu_irq_enable();
}


void LAN2Serial(uint8_t config)	
{
	uint32_t Baudrate =0;
	bool Parity=0;
	fifo_init(&fifo_desc, fifo_buffer, FIFO_BUFFER_LENGTH); //FiFo Init requiered for the USART
	fifo_init(&fifo_desc_usart_tx, fifo_buffer_usart_tx, FIFO_BUFFER_LENGTH); //FiFo Init requiered for the USART
	//Okay lets see what the User wants
	switch(config&0x07)
	{
		case 0:
		{
			Baudrate=115200;
		}
		break;
		
		case 1:
		{
			Baudrate=57600;
		}
		break;
		
		case 2:
		{
			Baudrate=38400;
		}
		break;
		
		case 3:
		{
			Baudrate=19200;
		}
		break;
		
		case 4:
		{
			Baudrate=9600;
		}
		break;
		
		case 5:
		{
			Baudrate=4800;
		}
		break;
		
		case 6:
		{
			Baudrate=2400;
		}
		break;
		
		case 7:
		{
			Baudrate=1200;
		}
		break;
		
		default:
		{
			Baudrate=115200;
		}
		break;
	}
	
	if((config&0x08)>0)
	{
		Parity=1;
	}
	
	usart_setup(Baudrate,Parity); //Make USART on the module work

	while(1==1) //Super loop for the LAN2Serial operation
	{
		tcp_2_usart(L2S_Socket,Buffer,Port);
		usart_2_tcp(L2S_Socket);
		//If we need to implement a web server this might come tricky here
	}	
	
}
	


int32_t tcp_2_usart(uint8_t sn, uint8_t* buf, uint16_t port) //Copy DATA from W5500 to USART
{
	int32_t ret;						//Variabl requiered to Check if Function retuned an error or somthing eles
	uint16_t size = 0;					//size and sentsize are for calculating the requierd buffer for datarecive
	switch(getSn_SR(sn))				//Okay sn is our Socket for the W5500 it can be between zero and seven (eight sockets)
	//Lets see in witch State we are
	{
		case SOCK_ESTABLISHED :		//Someone has Opened a Connection to us
		if(getSn_IR(sn) & Sn_IR_CON) //Okay this is the first time we realize the connection
		{
			//If this was a Telnet-Console we would send a Welcome to the System :-)
			//In this case we flus the FIFO....
			fifo_flush(&fifo_desc);
			setSn_IR(sn,Sn_IR_CON);
		}
		if((size = getSn_RX_RSR(sn)) > 0) //And lets see if there is some Data wathing for us
		{
			//If more than zero bytes are waiting we want to get the Data out of the Buffer and process it
			if(size > BUF_SIZE_T2U) size = BUF_SIZE_T2U; //If there is more data waiting than our Buffer can handle we will set the size to our DATA_BUF_SIZE
			//OKay after that adjust we need to see what we can put in the fifo....
			if(fifo_get_free_size(&fifo_desc_usart_tx)<size) size=fifo_get_free_size(&fifo_desc_usart_tx);
			if(size>0)
			{
				ret = recv(sn,Buffer,size);	//This will start to Transfer Data from the W5500 Buffer to our MCU
				if(ret <= 0) return ret;	//If the Result is less than 1 we got a problem and just return
				//Okay
				for(uint16_t i=0;i<ret;i++) //Put the Data to USART
				{
					fifo_push_uint8(&fifo_desc_usart_tx,Buffer[i]);
				}
				usart_set_dre_interrupt_level(&USARTD0,USART_INT_LVL_MED);	
			}
			
		}
		break; //Okay with this state we are done and leave the switch statement
		case SOCK_CLOSE_WAIT : //okay we need to close the socket
		
		if((ret=disconnect(sn)) != SOCK_OK) return ret; //Close Socket
		
		break;
		case SOCK_INIT :
		//Start to Listen for a Socket Connection
		if( (ret = listen(sn)) != SOCK_OK) return ret;
		break;
		case SOCK_CLOSED:
		//Okay the Socket is closed
		if((ret=socket(sn,Sn_MR_TCP,port,0x00)) != sn)
		return ret;
		//Okay the Socket is back to
		break;
		default:
		break;
	}
	return 1;
}

uint32_t usart_2_tcp(uint8_t sn) //We only need the Socket here
{
	#define FIFO_BUFFER_SIZE 128
	int32_t ret;						//Variable required to Check if Function returned an error or something else
	uint16_t sendsize=0;				//sendsize and sendsize are for calculating the required buffer for data to transmitt
	uint8_t TX_Buffer[FIFO_BUFFER_SIZE];//Max FIFO_BUFFER_SIZE Bytes for TX
	uint8_t status=FIFO_ERROR;
	switch(getSn_SR(sn))				//Okay sn is our Socket for the W5500 it can be between zero and seven (eight sockets)
	//Lets see in witch State we are
	{
		case SOCK_ESTABLISHED :		//Someone has Opened a Connection to us
		{
			//Get Data from USART FiFo, we got at 9600 Baud every 104.16us one char
			//At 115k2 we would get on Char every 8.68us
			//As the MUC is at its official max we have one clock taking 31.25ns (32MHz)
			//This means we have at 9600 Baud 3333.12 Clocks between one Char
			//At 115200 Baud we got only 277.76 Clocks left
			//
			//Assuming the fifo we use we need at 115200 an complete run of this in less than 4,550,860.8 Clocks
			
			
			sendsize = 0;				//we set our sendsize to zero
			do
			{
				status = fifo_pull_uint8(&fifo_desc, &TX_Buffer[sendsize++]);
				
			} while ((status==FIFO_OK)&&(sendsize<FIFO_BUFFER_SIZE)); //We Transfer only max FIFO_BUFFER_SIZE Bytes at once at max
			
			if(sendsize<2)
			{
				//Fail nothing in the FIFO
			}
			else
			{
				ret = send(sn,TX_Buffer,sendsize-1);
				if(ret < 0)		//Okay if the result of our send function is less than zero we got a problem
				{
					close(sn);		//so we close the connection
					return ret;		//and return what happened
				}
			}
			
		}
		break; //Okay with this state we are done and leave the switch statement
		
		default:
		break;
	}
	return 1;
	
	
	
}
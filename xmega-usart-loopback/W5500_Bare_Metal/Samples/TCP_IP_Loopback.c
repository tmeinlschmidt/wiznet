/*
 * TCP_IP_Loopback.c
 *
 * Created: 23.03.2014 18:47:29
 * 
 */ 
//Local variables, take a bit of RAM from our MCU
//In this case we will eat 32 bytes for the MCU Buffer
#include <asf.h>
#include "TCP_IP_Loopback.h"
#include "../Ethernet/socket.h"

#define DATA_BUF_SIZE 32
uint8_t Buffer[DATA_BUF_SIZE];

//Function prototype
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

void TCP_IP_Loopback()
{
	uint8_t SocketNo_TCP_IP = 0;
	uint16_t Port			= 23; //Telnet
	while(1==1)
	{
		loopback_tcps(SocketNo_TCP_IP,Buffer,Port); //Loopback
	}
}

///////////////////////////////////////////////////////////////
// Loopback Test Example Code using ioLibrary_BSD			 //
//															 //
//	Code by Wiznet											 //
//  Visit www.wiznet.eu for more infos   					 //
//  Additional Comments in Source by CalM					 //
///////////////////////////////////////////////////////////////
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
	int32_t ret;						//Variabl requiered to Check if Function retuned an error or somthing eles
	uint16_t size = 0, sentsize=0;		//size and sentsize are for calculating the requierd buffer for datarecive
	switch(getSn_SR(sn))				//Okay sn is our Socket for the W5500 it can be between zero and seven (eight sockets)
	//Lets see in witch State we are 
	{
		case SOCK_ESTABLISHED :		//Someone has Opened a Connection to us
		if(getSn_IR(sn) & Sn_IR_CON) //Okay this is the first time we realize the connection 
		{
			//If this was a Telnet-Console we would send a Welcome to the System :-)
			setSn_IR(sn,Sn_IR_CON);
		}
		if((size = getSn_RX_RSR(sn)) > 0) //And lets see if there is some Data wathing for us
		{
			//If more than zero bytes are waiting we want to get the Data out of the Buffer and process it
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; //If there is more data waitung than our Buffer can handle we will set the size to our DATA_BUF_SIZE
			ret = recv(sn,buf,size);	//This will start to Transfer Data from the W5500 Buffer to our MCU
			if(ret <= 0) return ret;	//If the Result is less than 1 we got a problem and just return 
			sentsize = 0;				//else we set our sentsize to zero
			while(size != sentsize)		//okay with the sentszize and size we received we transmit until the amount of received data is retransmitted
			{
				ret = send(sn,buf+sentsize,size-sentsize);
				if(ret < 0)		//Okay if the result of our send function is less than zero we got a problem
				{
					close(sn);		//so we close the connection
					return ret;		//and return what happend
				}
				sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
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
/*
 * TMC2300.c
 *
 * Created: 27.01.2020 18:00:54
 * Author: Dinar Talibullin
 */ 

#include "string.h"
#include "TMC2300.h"


static uint8_t send_data[TMC2300_BUF_CNT];

void swuart_calcCRC(unsigned char* datagram, unsigned char datagramLength)
{
	int i, j;
	unsigned char* crc = datagram + (datagramLength - 1); // CRC located in last byte of message
	unsigned char currentByte;
	*crc = 0;
	for (i = 0; i < (datagramLength - 1); i++) { // Execute for all bytes of a message
		currentByte = datagram[i]; // Retrieve a byte to be sent from Array
		for (j = 0; j < 8; j++) {
			if ((*crc >> 7) ^ (currentByte & 0x01)) // update CRC based result of XOR operation
			{
				*crc = (*crc << 1) ^ 0x07;
			}
			else
			{
				*crc = (*crc << 1);
			}
			currentByte = currentByte >> 1;
		} // for CRC bit
	} // for message byte
}

uint8_t tmc2300_write(uint8_t slaveaddr, uint8_t regaddr, uint32_t data)
{
	if (!tmc2300_get_uart_status())
	{
		tmc2300_set_uart_status(1);
		send_data[0]=TMC2300_SYNC;
		send_data[1]=slaveaddr;
		send_data[2]=TMC2300_WRITE_ADDR|regaddr;
		memcpy(&send_data[3], &data, 4);
		swuart_calcCRC(send_data, 8);
		tmc2300_uart_write(send_data, 8);
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t tmc2300_read(uint8_t slaveaddr, uint8_t regaddr, uint32_t* data)
{
	if (!tmc2300_get_uart_status())
	{
		tmc2300_set_uart_status(2);
		send_data[0]=TMC2300_SYNC;
		send_data[1]=slaveaddr;
		send_data[2]=regaddr;
		swuart_calcCRC(send_data, 4);
		tmc2300_uart_write(send_data, 4);
		while (tmc2300_get_uart_status()){}
		if(tmc2300_get_recieved_data(data)==0)
		{
			return 0;
		}
		else
		{
			return 2;
		}
		
	}
	else
	{
		return 1;
	}	
}
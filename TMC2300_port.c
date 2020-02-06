/*
 * TMC2300_port.c
 *
 * Created: 29.01.2020 12:38:28
 * Author: Dinar Talibullin
 */ 

#include <asf.h>
#include "string.h"
#include "math.h"
#include "TMC2300.h"

volatile static uint8_t uart_status = 0;
volatile static uint8_t* uart_send_buf;
volatile static uint8_t uart_send_cnt = 0;
volatile static uint8_t uart_send_index = 0;
volatile static uint8_t uart_receive_buf[TMC2300_BUF_CNT];
volatile static uint8_t uart_receive_index = 0;

# define F_CPU 2000000UL

void tmc2300_init(void)
{
	double bsel;
	int BSCALE = -1; // 0% error \m/
	int baud = 9600;
	if ( BSCALE >= 0 )
	{
		bsel = F_CPU/(16*(double)baud)-1;
	}
	else
	{
		bsel = (1<<-BSCALE)*(F_CPU/(16*(double)baud)-1);
	}
	//USARTE0 IR
	sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_USART0);
	PORTE.DIRSET = PIN3_bm;
	PORTE.OUTCLR = PIN3_bm;
	PORTE.DIRCLR = PIN2_bm;
	PORTE.OUTCLR = PIN2_bm;
	USARTE0.BAUDCTRLB = (int)round(bsel)>>8 | BSCALE<<USART_BSCALE_gp;
	USARTE0.BAUDCTRLA = (int)round(bsel) & 0xFF;
	USARTE0.CTRLA = USART_RXCINTLVL_LO_gc | USART_TXCINTLVL_LO_gc;
	USARTE0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
	//USARTE0.CTRLB=(USART_RXEN_bm|USART_TXEN_bm);
	USARTE0.CTRLB=USART_TXEN_bm;
}

void tmc2300_uart_write(uint8_t* p, uint8_t n)
{
	while((USARTE0.STATUS & USART_DREIF_bm) == false){}
	uart_send_buf=p;
	uart_send_cnt=n;
	USARTE0.CTRLB&= ~(USART_RXEN_bm);
	USARTE0.CTRLA |= USART_DREINTLVL_LO_gc;
}

/*
void serial_ir_send(char* cmd)
{
	//USARTD0.CTRLA |= USART_DREINTLVL_LO_gc;
	if (cmd[0] != '\0')
	{
		PORTE.OUTSET = PIN0_bm;
		while((USARTE0.STATUS & USART_DREIF_bm) == false){}
		strncpy((char*)ir_send_cmd, cmd, SERIAL_BUF_LEN);
		strcat ((char*)ir_send_cmd, "\n");
		ir_send_index = 0;
		//USARTD0.CTRLB&= ~(USART_RXEN_bm);
		USARTE0.CTRLA |= USART_DREINTLVL_LO_gc;
	}
}
*/
/*
char* serial_ir_read(void)
{
	strncpy((char*)ir_cmd_copy, (char*)ir_cmd, SERIAL_BUF_LEN);
	ir_cmd[0]='\0';
	return (char*)ir_cmd_copy;
}
*/
ISR(USARTE0_RXC_vect)
{
	if (uart_status==3)
	{
		uart_receive_buf[uart_receive_index] = USARTE0.DATA;
		uart_receive_index++;
		if (uart_receive_index == 8)
		{
			uart_status=0;
			uart_receive_index=0;
			USARTE0.CTRLB&=~(USART_RXEN_bm);			
		}
	}
	else
	{
		uart_receive_buf[uart_receive_index] = USARTE0.DATA;
	}
}

ISR(USARTE0_DRE_vect)
{
	USARTE0.DATA=uart_send_buf[uart_send_index];
	uart_send_index++;
	if (uart_send_index==uart_send_cnt)
	{
		uart_send_index=0;
		USARTE0.CTRLA &= ~(USART_DREINTLVL_LO_gc);
	}
}

ISR(USARTE0_TXC_vect)
{
	if (uart_status==1)
	{
		uart_status=0;
	}
	else
	{
		uart_status=3;
		USARTE0.CTRLB|=USART_RXEN_bm;
	}	
}

uint8_t tmc2300_get_uart_status(void)
{
	return uart_status;
}

void tmc2300_set_uart_status(uint8_t s)
{
	uart_status = s;
}

uint8_t tmc2300_get_recieved_data(uint8_t* p)
{
	uint8_t crc_c=uart_receive_buf[TMC2300_BUF_CNT-1];
	swuart_calcCRC(uart_receive_buf, TMC2300_BUF_CNT);
	if (uart_receive_buf[TMC2300_BUF_CNT-1]==crc_c)
	{
		p[0]=uart_receive_buf[6];
		p[1]=uart_receive_buf[5];
		p[2]=uart_receive_buf[4];
		p[3]=uart_receive_buf[3];
		//memcpy(p, &uart_receive_buf[3], 4);
		return 0;
	}
	else
	{
		return 1;
	}
}
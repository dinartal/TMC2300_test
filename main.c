/*
 * tmc2300.c
 *
 * Created: 24.01.2020 12:34:21
 * Author : Dinar Talibullin
 */ 

#include <asf.h>
#include "stdio.h"
#include "string.h"
#include "mcu_os.h"
#include "stepper.h"
#include "TMC2300.h"

void toggle(void);
void toggle2(void);
void toggle3(void);
void toggle4(void);

void stepper_task_move(void);
uint32_t d;

int main(void)
{
	//init clock
	sysclk_init();
	
	//init PMIC
	irq_initialize_vectors();
	
	udc_start();
	
	//enable interrupts
	cpu_irq_enable();	
	
	PORTF.DIRCLR=PIN2_bm;
	
	stepper_init();
	tmc2300_init();
	//STEPPER_ENABLE_PORT.OUTSET=STEPPER_ENABLE_PIN;
	//tmc2300_write(TMC2300_SLAVEADDR, TMC2300_ADDR_VACTUAL, 0x13008001);
	tmc2300_write(TMC2300_SLAVEADDR, TMC2300_ADDR_CHOPCONF, 0x13008001|(0x08<<TMC2300_CHOPCONF_MRES_bp));
	int err=0;
	err=mcu_os_add_task(stepper_task_move, 200);
	err=mcu_os_add_task(stepper_task, 1);
	mcu_os_init();
}

void stepper_task_move(void)
{
	stepper_move(200);
	tmc2300_read(TMC2300_SLAVEADDR, TMC2300_ADDR_SG_VALUE, &d);
	if (d>5)
	{
		char b[10];
		sprintf(b, "%d\n", d);
		udi_cdc_multi_write_buf(0, b, strlen(b));
	}	
}


void user_callback_vbus_action(bool b_vbus_high)
{
	
}

void user_callback_sof_action(void)
{
	
}

void user_callback_suspend_action(void)
{
	
}

void user_callback_resume_action(void)
{
	
}

bool my_callback_cdc_enable(uint8_t port)
{
	return 1;
}

void my_callback_cdc_disable(uint8_t port)
{
	
}

void my_callback_rx_notify(uint8_t port)
{

}

void my_callback_tx_empty_notify(uint8_t port)
{

}

void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg)
{
	
}

void my_callback_cdc_set_dtr(uint8_t port, bool b_enable)
{
	
}

void my_callback_cdc_set_rts(uint8_t port, bool b_enable)
{
	
}
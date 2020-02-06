/*
 * stepper.c
 *
 * Created: 27.01.2020 15:38:44
 * Author: Dinar Talibullin
 */ 

#include <asf.h>
#include "stepper.h"

static uint32_t steps = 0;

void stepper_init(void)
{
	STEPPER_ENABLE_PORT.DIRSET=STEPPER_ENABLE_PIN;
	STEPPER_ENABLE_PORT.OUTCLR=STEPPER_ENABLE_PIN;
	STEPPER_STEP_PORT.DIRSET=STEPPER_STEP_PIN;
	STEPPER_STEP_PORT.OUTCLR=STEPPER_STEP_PIN;
	STEPPER_DIR_PORT.DIRSET=STEPPER_DIR_PIN;
	STEPPER_DIR_PORT.OUTCLR=STEPPER_DIR_PIN;
}

void stepper_task(void)
{
	static uint8_t stepper_phase=0;
	static uint8_t enable_delay=0;
	static uint8_t step_phase=0;
	switch(stepper_phase)
	{
		case 0:
		if (steps)
		{
			STEPPER_ENABLE_PORT.OUTSET=STEPPER_ENABLE_PIN;
			stepper_phase++;
		}
		break;
		case 1:
		if (++enable_delay>10)
		{
			stepper_phase++;
			enable_delay=0;
		}
		break;
		case 2:
		if (steps)
		{
			if (step_phase)
			{
				step_phase=0;
				STEPPER_STEP_PORT.OUTSET=STEPPER_STEP_PIN;
			}
			else
			{
				STEPPER_STEP_PORT.OUTCLR=STEPPER_STEP_PIN;
				step_phase=1;
				steps--;
			}
		}
		else
		{
			stepper_phase++;
		}
		break;
		case 3:
		if (++enable_delay>10)
		{
			stepper_phase++;
			enable_delay=0;
		}
		break;
		case 4:
		STEPPER_ENABLE_PORT.OUTCLR=STEPPER_ENABLE_PIN;
		stepper_phase=0;
		break;				
	}	
}

void stepper_move(int32_t s)
{
	if (s>0)
	{
		STEPPER_DIR_PORT.OUTCLR=STEPPER_DIR_PIN;
	}
	else if (s<0)
	{
		STEPPER_DIR_PORT.OUTSET=STEPPER_DIR_PIN;
	}
	steps=abs(s);
}
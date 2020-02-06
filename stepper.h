/*
 * stepper.h
 *
 * Created: 24.01.2020 12:37:55
 * Author: Dinar Talibullin
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

#define STEPPER_ENABLE_PORT	PORTD
#define STEPPER_ENABLE_PIN	PIN0_bm
#define STEPPER_STEP_PORT	PORTE
#define STEPPER_STEP_PIN	PIN0_bm
#define STEPPER_DIR_PORT	PORTE
#define STEPPER_DIR_PIN		PIN1_bm


void stepper_init(void);
void stepper_task(void);
void stepper_move(int32_t s);



#endif /* STEPPER_H_ */
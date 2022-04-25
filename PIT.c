/*
 * PIT.c
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */
#include "derivative.h"
#include "PIT.h"
#include "TPM.h"

unsigned char pit_off = 0;


void PIT_IRQHandler(void){
	PIT_TFLG0 |= 1; //Clear flag
	GPIOB_PCOR |= 1; // Turn off trig
	PIT_TCTRL0 &= ~(1<<0);  //Disable timer
	pit_off = 1; // Indicate trig is off
}

void PIT_init(void){
	SIM_SCGC6 |= (1<<23);   // Clock enable for PIT module
	
	PIT_MCR &= ~(1<<1);      // Enable module, necessary to start configuring
	PIT_LDVAL0 = 250;     // Clk 24MHz -> Cuentas de 42ns
						// Para 10 us -> 240 cuentas, usamos 250 para no errar
	PIT_TCTRL0 |= (1<<1); //Enable Interrupt
	NVIC_ISER |= (1<<22); 
}

void send_trigger(void){
	TPM_init();
	GPIOB_PSOR = 1; // Send trig -> HIGH
	PIT_TCTRL0 |= 1; //Enable timer
}


/*
 * TPM.c
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */
#include "derivative.h"
#include "TPM.h"
#include "PIT.h"

unsigned char rising = 0;
unsigned short t2 = 0;
//PWM variables 
unsigned char interrupt_state=0;
unsigned long fsA_count = 0;
unsigned long fsB_count = 0;
unsigned long fsC_count = 0;
unsigned long fsD_count = 0;
unsigned long periodo = 50000;
unsigned short duty_cycle = 0;
unsigned short duty_cycle2 = 0;
// US variables
int distance =0;
int time = 0;

void FTM1_IRQHandler(void){
	TPM1_C1SC |= (1<<7); //Clears flag
	if(rising==0){
		TPM1_CNT = 0x0000;
		rising=1;
	}else if(rising == 1){
		t2 = TPM1_C1V; // Save falling time
		TPM1_CNT = 0x0000;
		time = (t2*250)/1000;  //El 1000 nos da el tiempo en us. Por 250 porque cada cuenta vale 250ns
		distance = time/58;
		rising=0;
		
		if(distance<20){
			duty_cycle=0;
			duty_cycle2=0;
			TPM_change_dc();
		}
		send_trigger();
	}
}

void FTM0_IRQHandler(void){
	
	if (((TPM0_C0SC & (1<<7)) >> 7) == 1){
		interrupt_state = 1;
		TPM0_C0SC|=(1<<7);
		fsA_count++;
	}
	if (((TPM0_C1SC & (1<<7)) >> 7) == 1){
		interrupt_state = 2;
		TPM0_C1SC|=(1<<7);	
		fsB_count++;
	}	
	if (((TPM0_C2SC & (1<<7)) >> 7) == 1){
		interrupt_state = 3;
		TPM0_C2SC|=(1<<7);	
		fsC_count++;
	}	
	if (((TPM0_C3SC & (1<<7)) >> 7) == 1){
		interrupt_state = 4;
		TPM0_C3SC|=(1<<7);	
		fsD_count++;
	}	
}

void TPM0_init(void){
	SIM_SCGC6 |= (1<<26) + (1<<25) + (1<<24); // Clock enable for TPM2, TPM1 and TPM0 module
	SIM_SOPT2 |= (3<<24); // MCGIRCLK
	
	//TPM0
	TPM0_SC=(1<<3);	// LPTPM counter increments on every LPTPM counter clock
	TPM0_C0SC=(1<<2)+(1<<6);        //input capture rising edge, hab intr channel
	TPM0_C1SC=(1<<2)+(1<<6);        //input capture rising edge, hab intr channel
	TPM0_C2SC=(1<<2)+(1<<6);        //input capture rising edge, hab intr channel
	TPM0_C3SC=(1<<2)+(1<<6);        //input capture rising edge, hab intr channel
	NVIC_ISER|=(1<<17);
}

void TPM_init(void){
	// TPM1
	TPM1_SC |= (1<<3); // LPTPM counter increments on every LPTPM counter clock
	TPM1_C1SC |= (3<<2) + (1<<6); // Input capture in rising and falling edges + channel interrupt enable
	TPM1_CONF |= (3<<6); //Count at debugger mode
	NVIC_ISER |= (1<<18);
}

void TPM2_init(void){
	
	//TPM2
	TPM2_SC=(1<<3)+3; // LPTPM counter increments on every LPTPM counter clock	
	TPM2_MOD=periodo;
	TPM2_C0SC=(9<<2); 			//EPWM set on match
	TPM2_C1SC=(9<<2);
	TPM2_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	TPM2_C1V=((periodo*(100-duty_cycle2))/100);
	
}

void TPM_change_dc(void){
	TPM2_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	TPM2_C1V=((periodo*(100-duty_cycle2))/100);
}


	
	


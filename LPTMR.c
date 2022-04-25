/*
 * LPTMR.c
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */

#include "derivative.h"
#include "LPTMR.h"
#include "TPM.h"
#include "I2C.h"

extern unsigned long fsA_count;
extern unsigned long fsB_count;
extern unsigned long fsC_count;
extern unsigned long fsD_count;

unsigned long rpm = 0;
unsigned long rpm_promedio = 0;
unsigned long rpm_promedio2 = 0;
unsigned long rpm_promedio3 = 0;
unsigned long rpm_promedio4 = 0;
unsigned long rpm_promedio5 = 0;
int rpm_promedio6 = 0;

unsigned long rpm_s = 0;
unsigned long rpm_promedio_s = 0;
unsigned long rpm_promedio2_s = 0;
unsigned long rpm_promedio3_s = 0;
unsigned long rpm_promedio4_s = 0;
unsigned long rpm_promedio5_s = 0;
int rpm_promedio6_s = 0;

void LPTimer_IRQHandler(void){	
	unsigned long rpmA = 0;
	unsigned long rpmB = 0;
	unsigned long rpmC = 0;
	unsigned long rpmD = 0;
		
	LPTMR0_CSR|=(1<<7);
	
	rpmA = (60*fsA_count*1000*10)/(11*468*20);
	rpmB = (60*fsB_count*1000*10)/(11*468*20);
	rpmC = (60*fsC_count*1000*10)/(11*468*20);
	rpmD = (60*fsD_count*1000*10)/(11*468*20);
	
	rpm = (rpmA+rpmB)/2;
	rpm_promedio = (rpm_promedio+rpm)/2;
	rpm_promedio2 = (rpm_promedio+rpm_promedio2)/2;
	rpm_promedio3 = (rpm_promedio2+rpm_promedio3)/2;
	rpm_promedio4 = (rpm_promedio3+rpm_promedio4)/2;
	rpm_promedio5 = (rpm_promedio4+rpm_promedio5)/2;
	rpm_promedio6 = (rpm_promedio5+rpm_promedio6)/2;
	
	rpm_s = (rpmC+rpmD)/2;
	rpm_promedio_s = (rpm_promedio_s+rpm_s)/2;
	rpm_promedio2_s = (rpm_promedio_s+rpm_promedio2_s)/2;
	rpm_promedio3_s = (rpm_promedio2_s+rpm_promedio3_s)/2;
	rpm_promedio4_s = (rpm_promedio3_s+rpm_promedio4_s)/2;
	rpm_promedio5_s = (rpm_promedio4_s+rpm_promedio5_s)/2;
	rpm_promedio6_s = (rpm_promedio5_s+rpm_promedio6_s)/2;
	
	fsA_count = 0;
	fsB_count = 0;
	fsC_count = 0;
	fsD_count = 0;
	
	//IMU_data();
	//Process_Data();
}

void LPTMR_init(void){
	SIM_SCGC5|=1;
	
	LPTMR0_CMR=20-1;		// Conversion ADC cada 20 ms
	LPTMR0_PSR=5;			//LPO, bypass preescaler
	LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer
	
	NVIC_ISER|=(1<<28);		//hab intr LPT desde NVIC
}

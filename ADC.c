/*
 * ADC.c
 *
 *  Created on: Nov 21, 2020
 *      Author: SCADA_Admin
 */

#include "derivative.h" /* include peripheral declarations */
#include "Math.h"
#include "ADC.h"


// This codes implements an ADC conversion and prints the value in Serial Monitor
// The analog signal comes from a temperature sensor -> ZTP-135SR

int adc_result_raw = 0;
int adc_result_nv = 0;
int temperatura=0;
char send_temp_ir[5] = {0};

void ADC_init(void){
	//ADC clocking -> Bus clock
	
	SIM_SCGC6 |= (1<<27); //Enable clock for ADC
	
	// ADC config
	ADC0_SC1A &= ~(1<<6)     //Desabilitamos AIEN 
			  |  31;         //ADCH = 31 que en binario es 1111 para deshabilitar  el modulo
	
	ADC0_CFG1 |= (1<<2); // 12-bit conversion, Clock Bus clk, 
	
	ADC0_SC2 = 0X00000000;   // Software trigger ADC
	ADC0_SC3 = 0X00000000;   // No calibration
}

void get_temp(void){
	
	ADC0_SC1A &= ~(31<<0); // Clear channel before writing
	ADC0_SC1A |= 6;        // Activate software trigger, select channel 6
	
	do{}while(((ADC0_SC1A & (1<<7))>>7)==0); //Wait until CoCo
	adc_result_raw = ADC0_RA;
	adc_result_nv = 3300*adc_result_raw/4095; // Convert to mV
	temperatura = (sqrt(((adc_result_nv+3000)/0.9375)+pow((93.75/1.875),2))-(93.75/1.875))*100;
	
	temperatura = round(temperatura);
	temperatura = temperatura/10;
	send_temp_ir[4] = 'C';
	send_temp_ir[3] = ((int)temperatura%10) + '0';
	temperatura/=10;
	send_temp_ir[2] = '.';
	send_temp_ir[1] = ((int)temperatura%10) + '0';
	send_temp_ir[0] = (temperatura/10) + '0';
}

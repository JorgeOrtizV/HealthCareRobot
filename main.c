/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "PIT.h"
#include "TPM.h"
#include "UART.h"
#include "LPTMR.h"
#include "WiFi.h"
#include "I2C.h"
#include "ADC.h"

/* 
 * This project implements an ultrasound sensor to detect whether an object is present or not, without taking
 * into consideration distance measurement.
 * 
 * Clocks configuration:
 * MCGFLLCLK: 48 MHz, MCGIRCLK: 4MHz, BusClk: 24MHz
 * BusClk -> PIT
 * MCGIRCLK -> TPM1
 * MCGFLLCLK -> UART0
 * 
 * PORTB 0 -> GPIO Output -> Trigger
 * PORTB 1 -> TPM1_CH1 -> Alt 3 -> Echo
 * 
 * PORTA 1 -> UART0 RX -> Alt 2
 * PORTA 2 -> UART0 TX -> Alt 2
 * 
 * 
 * 
 * ENCODERS
 * PORTC 1 -> TPM0_C0 -> Alt 4
 * PORTC 2 -> TPM0_C1 -> Alt 4
 * PORTE 29 -> TPM0_C2 -> Alt 3
 * PORTE 30 -> TPM0_C3 -> Alt3
 * 
 * PORTE 22 -> PWM
 * PORTE 23 -> PWM 2
 * 
 * 
 * 
 * WiFi
 * PORTE 0 as TX -> UART1
 * PORTE 1 as RX -> UART1
 * PORTB 18 -> Ouput GPIO -> Start signal WiFi
 * 
 * 
 * 
 * I2C
 * PORTB 2 -> SCL -> Alt2
 * PORTB 3 -> SDA -> Alt2
 * 
 * 
 * 
 * ADC
 * PORTD 5 -> ADC0_SE6b
 * 
 */

//extern int rpm_promedio6;

void PORT_init(void){
	SIM_SCGC5 |= (1<<10) + (1<<9) + (1<<11) + (1<<12) + (1<<13); //Clock enable for ports A, B, C, D and E
	PORTB_PCR0 = (1<<8);
	
	
	// uart pins
	PORTA_PCR1 = (2<<8);
	PORTA_PCR2 = (2<<8);
	
	// uart 1 pins -> WiFi
	// TX & RX
	PORTE_PCR0 = (3<<8);                //PORTE 0 as TX
	PORTE_PCR1 = (3<<8);                //PORTE 1 as RX
	
	//TPM pins
	PORTB_PCR1 = (3<<8); // Alt 3 -> TPM -> US Echo
	
	PORTC_PCR1=(1<<10); //TPM0_C0 -> Encoder
	PORTC_PCR2=(1<<10); //TPM0_C1 -> Encoder
	PORTE_PCR29=(3<<8); //TPM0_C2 -> Encoder
	PORTE_PCR30=(3<<8); //TPM0_C3 -> Encoder
	
	PORTE_PCR22=(3<<8);	//TMP2_CH0 -> PWM
	PORTE_PCR23=(3<<8); //TMP2_CH1 -> PWM
	
	GPIOB_PDDR |= 1; // PORTB 0 -> Output
	
	//Red LED -> WiFi
	PORTB_PCR18 = (1<<8);               //PORTB 18 as GPIO
	GPIOB_PDDR |= (1<<18);              //B18 as output
	GPIOB_PSOR |= (1<<18);              //Turns off led
	
	// I2C
	PORTB_PCR2 = (2<<8);// I2C0 SCL
	PORTB_PCR3 = (2<<8);// I2C0 SDA
	
	// ADC pins
	PORTD_PCR5 = 0; //Default function
}

void clock_init(void){
	MCG_C1 |=  0<<6;						// CLKS = 0: MCGOUTCLK Source is FLL (default) 
	MCG_C1 |= (1<<2) + (1<<1);				// IREFS = 1: Slow internal reference clock (default) for FLL & MCGIRCLK active
	MCG_C2 |= 1;					        //Mux IRCLK : FIRC (4 MHz) pag 116
	MCG_SC = 0;					            //Preescaler FIRC 1:1 pag 116
	MCG_C4 |= (1<<5) + (1<<7);				//DRTS_DRS = 1: Mid range & DCO range 48MHz
	
	SIM_CLKDIV1 = 0x00010000;               // OUTDIV4 = 1 -> BUSCLK = FLL/2
}


int main(void){
	
	clock_init();
	PORT_init();
	PIT_init();
	UART_init();
	UART1_init();
	TPM0_init();
	I2C_init();
	LPTMR_init();
	ADC_init();
	
	WiFi_setup();
	
	TPM2_init();
	
	send_trigger();
	
	IMU_data();
	
	while(1){
		WiFi_execute_instructions();
		/*send_trigger();
		do{}while(pit_off==0);
		do{}while(t2==0);
		time = (t2*250)/1000;  //El 1000 nos da el tiempo en us. Por 250 porque cada cuenta vale 250ns
		distance = time/58;
		if(distance<20){
			do{}while(clear==0);
			duty_cycle = 60;                         
			duty_cycle2 = 60;
		}
		pit_off = 0;*/
		//UART_write(rpm_promedio6);
	}

	return 0;
}

/*
 * UART.c
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */
#include "derivative.h"
#include "UART.h"

void UART_init(void){
	SIM_SCGC4 |= (1<<10); //Clock enable for UART0 module
	SIM_SOPT2 |= (1<<26); //MCFLLCLK for UART0
	
	//UART SET UP
	UART0_C4 |= 15; //Default OSR
	
	// Baud rate = 24MHz/(16*115200) = 13
	UART0_BDL = 0x1A;
	UART0_BDH = 0;
	UART0_C1 = 0x00; // m=0 -> Trama 8 bits
	UART0_C2 |= (1<<3); //transmision enable
}

void UART_write(int data){
	int indx = 0;
	unsigned char string[4] = {0};
	
	//ITOA
	string[3] = (data%10) + '0';
	data /= 10;
	string[2] = (data%10) + '0';
	data /= 10;
	string[1] = (data%10) + '0';
	string[0] = (data/10) + '0';
	
	do{
		while((UART0_S1 & (1<<7)) == 0);         //Wait until the transmission buffer is empty
		UART0_D = string[indx++];
	}while(string[indx] != '\0');                //Waiting for null character
	
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\r';                                //Return
	while((UART0_S1 & (1<<7)) == 0);             
	UART0_D = '\n';                                //New line
}

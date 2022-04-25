/*
 * WiFi.c
 *
 *  Created on: Oct 30, 2020
 *      Author: SCADA_Admin
 */
#include "derivative.h"
#include "WiFi.h"
#include "TPM.h"
#include "LPTMR.h"
#include "I2C.h"
#include "ADC.h"

unsigned char id;
char buffer[500] = "";
char to_parse[100] = "";
unsigned char found_char = 0;
unsigned char buff_ind = 0;
unsigned char messageComplete = 0;

// PWM
extern unsigned long periodo;
extern unsigned short duty_cycle;
extern unsigned short duty_cycle2;

//Us
extern int distance;
extern int clear;

//Encoders
extern int rpm_promedio6;
extern int rpm_promedio6_s;

//I2C
extern char send_gyrox[2];
extern char send_gyroy[2];
extern char send_gyroz[2];
extern char send_accx[5];
extern char send_accy[5];
extern char send_accz[5];
extern char send_temp[5];

//ADC
extern char send_temp_ir[5];

void UART1_init(){
	SIM_SCGC4 |= (1<<11);                       //clk UART1
	
	UART1_BDH = 0;                              //baud rate = 115200
	UART1_BDL = 13;                            
	
	//Baud rate = CLK/(OSR+1)*SBR
	
	UART1_C1 = 0;                               //8 data bits, no parity, 1 bit stop
	UART1_C2 |= (3<<2);                         //TE = 1, transmitter enabled
												//RE = 1, receiver enabled
												//TIE = 0, RIE = 0, 	interrupts disabled
	NVIC_ISER |= (1<<13);                       //Intr NVIC UART1
}


//======================UART0 FUNCTIONS=====================================================

void UART0_write(char string[]){
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
	unsigned char i = 0;
	do{
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = string[i++];
	}while(string[i] != '\0');
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\r';
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
}

void UART0_writeChar(unsigned char c){
	while ((UART0_S1 & (1<<7)) == 0);
	UART0_D = c;
}

/*void UART0_IRQHandler(){
	if (UART0_S1 & (1<<5)){
		char temp = UART0_D;
		if (temp != 13){
			dato_read[ir_u0] = temp;
			ir_u0++;
		}else{
			dato_read[ir_u0] = '\0';
			ir_u0 = 0;
			messageComplete = 1;
		}
	}
}*/

//========================UART0 FUNCITONS================================================

//========================UART1 FUNCTIONS================================================

void UART1_write(char string[]){
	UART0_write(string);
	unsigned char i = 0;
	do{
		while((UART1_S1 & (1<<7)) == 0);
		UART1_D = string[i++];
	}while(string[i] != '\0');
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_writeChar(unsigned char c){
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = c;
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_read(char string[]){
	unsigned char i = 0;
	while(string[i] != '\0'){
		to_parse[i] = string[i];
		i++;
	}
	to_parse[i] = '\0';
	messageComplete = 0;
	buff_ind = 0;
	UART1_C2 |= (1<<5);                      //intr enabled
}

void UART1_IRQHandler(){
	if (UART1_S1 & (1<<5)){
		char temp = UART1_D;                     //Erasing flag by reading
		UART0_writeChar(temp);
		buffer[buff_ind] = temp;                 //Saving read message into buffer
		buff_ind++;
		if (temp == to_parse[found_char]){       //Coincidence found
			found_char++;
			if (to_parse[found_char] == '\0'){   //Whole string found
				buffer[buff_ind] = '\0';
				messageComplete = 1;
				UART1_C2 &= ~(1<<5);             //intr disabled
			}
		}
		else{
			found_char = 0;                      //Looking up again
		}
	}
}
//=========================UART1 FUNCTIONS=================================================

//=========================WIFI FUNCTIONS==================================================

void WiFi_setup(){
	UART1_write("AT+RST");                                    //Resetear el modulo
	UART1_read("ready");                                      //Esperar a que el modulo este listo
	while(!(messageComplete));
	UART1_write("AT");                                        //Confirmar que haya comunicacion
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWMODE=1");                               //Configurar como cliente
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWLAP");                                  //Mostar redes disponibles
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWJAP=\"INFINITUM0D9C_2.4\",\"a4WDXj3ec2\""); //Conectarse a red
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIFSR");                                  //Mostrar direccion IP
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPMUX=1");                               //Configurar para multiples conexiones
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPSERVER=1,80");                         //Configurar puerto
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_send(char id,char message[]){
	unsigned char len = 0;
	while(message[len] != '\0') len++;
	char open[] = "AT+CIPSEND=i,le";
	char close[] = "AT+CIPCLOSE=i";
	
	open[11] = id;
	open[13] = (len/10) + '0';
	open[14] = (len%10) + '0';
	close[12] = id;
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message);
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_sendInfo(void){
	unsigned char len = 0;
	unsigned char len2 = 0;
	char open[] = "AT+CIPSEND=i,le";
	char open2[] = "AT+CIPSEND=i,le";
	char close[] = "AT+CIPCLOSE=i";
	
	//DISTANCIA
	char string_dist[3] = {0};
	string_dist[2] = (distance%10) + '0';
	distance /= 10;
	string_dist[1] = (distance%10) + '0';
	string_dist[0] = (distance/10) + '0';
	//RPM1
	char string_rpm1[3] = {0};
	string_rpm1[2] = (rpm_promedio6%10) + '0';
	rpm_promedio6 /= 10;
	string_rpm1[1] = (rpm_promedio6%10) + '0';
	string_rpm1[0] = (rpm_promedio6/10) + '0';
	//RPM2
	char string_rpm2[3] = {0};
	string_rpm2[2] = (rpm_promedio6_s%10) + '0';
	rpm_promedio6_s /= 10;
	string_rpm2[1] = (rpm_promedio6_s%10) + '0';
	string_rpm2[0] = (rpm_promedio6_s/10) + '0';
	char message[] = "mensaje:ESTADO DEL AGV, distancia:###, RPM_der:###, RPM_izq:###, Gyro_x:##, Gyro_y:##";
	char message2[] = "Gyro_z:##, Acc_x:#####, Acc_y:#####, Acc_z:#####, Temp:#####";
	len = sizeof(message)/sizeof(message[0]);
	len2 = sizeof(message2)/sizeof(message2[0]);
	message[34] = string_dist[0];
	message[35] = string_dist[1];
	message[36] = string_dist[2];
	message[47] = string_rpm1[0];
	message[48] = string_rpm1[1];
	message[49] = string_rpm1[2];
	message[60] = string_rpm2[0];
	message[61] = string_rpm2[1];
	message[62] = string_rpm2[2];
	message[72] = send_gyrox[0];
	message[73] = send_gyrox[1];
	message[83] = send_gyroy[0];
	message[84] = send_gyroy[1];
	message2[7] = send_gyroz[0];
	message2[8] = send_gyroz[1];
	message2[17] = send_accx[0];
	message2[18] = send_accx[1];
	message2[19] = send_accx[2];
	message2[20] = send_accx[3];
	message2[21] = send_accx[4];
	message2[30] = send_accy[0];
	message2[31] = send_accy[1];
	message2[32] = send_accy[2];
	message2[33] = send_accy[3];
	message2[34] = send_accy[4];
	message2[43] = send_accz[0];
	message2[44] = send_accz[1];
	message2[45] = send_accz[2];
	message2[46] = send_accz[3];
	message2[47] = send_accz[4];
	message2[55] = send_temp[0];
	message2[56] = send_temp[1];
	message2[57] = send_temp[2];
	message2[58] = send_temp[3];
	message2[59] = send_temp[4];
	message2[72] = send_temp_ir[0];
	message2[73] = send_temp_ir[1];
	message2[74] = send_temp_ir[2];
	message2[75] = send_temp_ir[3];
	message2[76] = send_temp_ir[4];
	open[11] = id;
	open[13] = (len/10) + '0';
	open[14] = (len%10) + '0';
	close[12] = id;
	
	// Second message config
	open2[11] = id;
	open2[13] = (len2/10) + '0';
	open2[14] = (len2%10) + '0';
	close[12] = id;
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message); //Send first msg
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(open2);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message2); //Send second msg
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_execute_instructions(){
	char instruction[100] = "";
	char pre_instruction[] = "GET /";
	char pre_id[] = "+IPD,";
	unsigned char instruction_obtained = 0;
	unsigned char pre_id_found = 0;
	unsigned char pre_inst_found = 0;
	unsigned char inst_ind = 0;
	unsigned char i = 0;
	
	UART1_read(" HTTP/");
	while(!(messageComplete));
	
	//FINDING INSTRUCTION
	while (!(instruction_obtained)){
		if (pre_id_found){
			if (pre_inst_found){
				instruction[inst_ind] = buffer[i];
				inst_ind++;
				if (buffer[i] == '\0'){
					instruction_obtained = 1;
					instruction[inst_ind-7] = '\0';
				}
			}else{
				if (buffer[i] == pre_instruction[inst_ind]){           //Coincidence found
					inst_ind++;
					if (pre_instruction[inst_ind] == '\0'){            //Found GET /
						pre_inst_found = 1;
						inst_ind = 0;
					}
				}else inst_ind = 0;
			}
		}else{
			if (buffer[i] == pre_id[inst_ind]){                        //Coincidence found
				inst_ind++;
				if (pre_id[inst_ind] == '\0'){                         //Fount +IPD,
					pre_id_found = 1;
					id = buffer[++i];
					inst_ind = 0;
				}
			}else inst_ind = 0;
		}
		i++;
	}
	
	switch (instruction[0]){
		case 'r':
		case 'R':
			GPIOB_PTOR |= (1<<18);                      //Toggle red led
			WiFi_send(id, "<h1> LED ROJO CAMBIO DE ESTADO! <h1/>");
			// Indicates com ready
			break;
		case 'w':
		case 'W':                                      //Moving forward
			duty_cycle = 60;                         
			duty_cycle2 = 60;
			break;
		case 's':
		case 'S':									   //Stop moving
			duty_cycle = 0;
			duty_cycle2 = 0;
			break;
		case 'a':
		case 'A':                                      //Turn left
			duty_cycle = 20;
			duty_cycle2 = 60;
			break;
		case 'd':
		case 'D':                                      //Turn right
			duty_cycle = 60;
			duty_cycle2 = 20;
			break;
		case 'c':
		case 'C':                                      //Check if path is clear
			if(distance>20){
				duty_cycle = 60;
				duty_cycle2 = 60;
				break;
			}else{
				break;
			}
		case 'e':
		case 'E':                                      //Command used to refresh data
			IMU_data();
			break;
		case 't':
		case 'T':                                      //Command used to refresh data
			IMU_data();
			get_temp();
			break;
		default:
			break;
	}
	TPM_change_dc();
	WiFi_sendInfo();
}

//=========================WiFi FUNCTIONS==================================================



/*
 * WiFi.h
 *
 *  Created on: Oct 30, 2020
 *      Author: SCADA_Admin
 */

#ifndef WIFI_H_
#define WIFI_H_


void UART1_init();
void UART0_write(char string[]);
void UART0_writeChar(unsigned char c);
void UART1_write(char string[]);
void UART1_writeChar(unsigned char c);
void UART1_read(char string[]);
void UART1_IRQHandler();
void WiFi_setup();
void WiFi_send(char id,char message[]);
void WiFi_sendInfo(void);
void WiFi_execute_instructions();




#endif /* WIFI_H_ */

/*
 * PIT.h
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */

#ifndef PIT_H_
#define PIT_H_

void PIT_IRQHandler(void);
void PIT_init(void);
void send_trigger(void);


#endif /* PIT_H_ */

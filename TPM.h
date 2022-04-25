/*
 * TPM.h
 *
 *  Created on: Nov 16, 2020
 *      Author: SCADA_Admin
 */

#ifndef TPM_H_
#define TPM_H_

void FTM1_IRQHandler(void);
void FTM0_IRQHandler(void);
void TPM0_init(void);
void TPM_init(void);
void TPM2_init(void);
void TPM_change_dc(void);

#endif /* TPM_H_ */

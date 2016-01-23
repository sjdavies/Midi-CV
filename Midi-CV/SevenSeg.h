/*
 * SevenSeg.h
 *
 * Created: 21/08/2014 22:58:55
 *  Author: sjdavies
 */ 


#ifndef SEVENSEG_H_
#define SEVENSEG_H_

#include "hal.h"

void seg7_clear_dp(uint8_t);
void seg7_display(uint16_t);
void seg7_display_char(uint8_t, uint8_t);
void seg7_display_dec(uint8_t, uint8_t);
void seg7_display_dp(uint8_t);
void seg7_display_next(void);
void seg7_display_raw(uint8_t, uint8_t);

#endif /* SEVENSEG_H_ */
/*
 * SevenSeg.c
 *
 * Created: 21/08/2014 22:25:20
 *  Author: sjdavies
 */ 

#include <stdint.h>

#include "SevenSeg.h"

/*
 * One byte per 7 seg digit.
 * Bit 7 = decimal point
 * Bits 6 - 0 = segments A-G (in sequence)
 *
 * NB. All digits are active low.
 */
volatile uint8_t gDigits[4] = { 0xFF, 0xFF, 0xFF, 0xFF };	// all off

void seg7_clear_dp(uint8_t segment)
{
	gDigits[segment] |= 0x80;	// active low
}

/*
 * Display an unsigned 16 bit number as 4 hex digits.
 */
void seg7_display(uint16_t value)
{
	for (int i = 0; i < 4; i++) {
		uint8_t v = ((value >> (4 * i)) & 0x0F);
		seg7_display_char(i, (v <= 9) ? ('0' + v) : ('A' + v - 10));
	}
}

void seg7_display_char(uint8_t index, uint8_t c)
{
	uint8_t digit;
	uint8_t dp = gDigits[index] & 0x80;
	
	switch (c) {
	case '-':
		digit = 0xFE;
		break;
	case '0':
	case 'O':
	case 'o':
		digit = 0x81;
		break;
	case '1':
		digit = 0xCF;
		break;
	case '2':
	case 'Z':
	case 'z':
		digit = 0x92;
		break;
	case '3':
		digit = 0x86;
		break;
	case '4':
		digit = 0xCC;
		break;
	case '5':
		digit = 0xA4;
		break;
	case '6':
		digit = 0xA0;
		break;
	case '7':
		digit = 0x8F;
		break;
	case '8':
		digit = 0x80;
		break;
	case '9':
	case 'G':
	case 'g':
		digit = 0x84;
		break;
	case 'A':
	case 'a':
		digit = 0x88;
		break;
	case 'B':
	case 'b':
		digit = 0xE0;
		break;
	case 'C':
	case 'c':
		digit = 0xB1;
		break;
	case 'D':
	case 'd':
		digit = 0xC2;
		break;
	case 'E':
	case 'e':
		digit = 0xB0;
		break;
	case 'F':
	case 'f':
		digit = 0xB8;
		break;
	case 'H':
	case 'h':
	case 'K':
	case 'k':
	case 'X':
	case 'x':
		digit = 0xC8;
		break;
	case 'I':
	case 'i':
		digit = 0xF9;
		break;
	case 'J':
	case 'j':
		digit = 0xC3;
		break;
	case 'L':
	case 'l':
		digit = 0xF1;
		break;
	case 'M':
	case 'm':
		digit = 0xAB;
		break;
	case 'N':
	case 'n':
		digit = 0xEA;
		break;
	case 'P':
	case 'p':
		digit = 0x98;
		break;
	case 'Q':
	case 'q':
		digit = 0x8C;
		break;
	case 'R':
	case 'r':
		digit = 0xFA;
		break;
	case 'S':
	case 's':
		digit = 0xA4;
		break;
	case 'T':
	case 't':
		digit = 0xF0;
		break;
	case 'U':
	case 'u':
		digit = 0xC1;
		break;
	case 'V':
	case 'v':
		digit = 0xE3;
		break;
	case 'W':
	case 'w':
		digit = 0xD5;
		break;
	case 'Y':
	case 'y':
		digit = 0xC4;
		break;
	case ' ':
		/* FALLTHRU */
	default:
		digit = 0xFF;
	}
	
	gDigits[index] = digit & (dp | 0x7f);
}

void seg7_display_dec(uint8_t number, uint8_t numDigits)
{
	uint8_t digit = '0' + (number % 10);
	seg7_display_char(0, digit);
	
	if (numDigits > 1) {
		number /= 10;
		digit = '0' + (number % 10);
		seg7_display_char(1, digit);
	}
	
	if (numDigits > 2) {
		number /= 10;
		digit = '0' + (number % 10);
		seg7_display_char(2, digit);
	}
}

void seg7_display_dp(uint8_t segment)
{
	gDigits[segment] &= 0x7f;
}

void seg7_display_next(void)
{
	static int i = 3;		// digit index, counts from 3 down to 0

	hal_disable_all_segments();
	hal_display_digit(gDigits[i]);
	hal_enable_segment(i--);
		
	if (i < 0) {
		i = 3;
	}
}

void seg7_display_raw(uint8_t index, uint8_t c)
{
	uint8_t dp = gDigits[index] & 0x80;	
	
	gDigits[index] = (c & 0x7F) | dp;
}

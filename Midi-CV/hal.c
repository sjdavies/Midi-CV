/*
 * hal.c
 *
 * Created: 21/08/2014 22:34:47
 *  Author: sjdavies
 *
 * Pins
 * ----
 * PA - 7 seg display (cathodes)
 *    - PA7 to DP
 *    - PA6 to A
 *    - PA5 to B
 *    - PA4 to C
 *    - PA3 to D
 *    - PA2 to E
 *    - PA1 to F
 *    - PA0 to G
 * PB - PB7-4 SPI (DAC), PB3-1 pushbuttons
 *    - PB7 to SCK
 *    - PB6 to MISO
 *    - PB5 to MOSI
 *    - PB4 to /CS
 *    - PB3 to Button3 (-)
 *    - PB2 to Button2 (+)
 *    - PB1 to Button1 (Sel)
 *    - PB0 Gate LED (active low)
 * PC - PC7 (spare)
 *    - PC6 (spare)
 *    - PC5 to TDI
 *    - PC4 to TDO
 *    - PC3 to TMS
 *    - PC2 to TCK
 *    - PC1 (spare)
 *    - PC0 (spare)
 * PD - PD7-4 7 seg display (anodes)
 *    - PD7 to SEGA (MSD)
 *    - PD6 to SEGB
 *    - PD5 to SEGC
 *    - PD4 to SEGD (LSD)
 *    - PD3 to Debug TxD
 *    - PD2 to Debug RxD
 *    - PD1 to MIDI TxD
 *    - PD0 to MIDI RxD
 */ 

#include <avr/interrupt.h>

#include "hal.h"

void hal_init(void)
{
	// 7 segment display
	DDRA = 0xFF;	// digit bits (active low)
	DDRD = 0xF0;	// common anodes (active low)
	
	// PB3-1 connected to pushbuttons
	// activate tie highs
	DDRB = 0x01;		// PB3-1 as input, PB0 as output
	PORTB = 0x0F;		// enable tie high, gate LED off
	
	// Timer 1 - expires every 5ms
	// used to multiplex display and sample pushbuttons
	TCNT1 = 0;
	OCR1A = 311;							// approx 200Hz
	TCCR1A = 0;								// not using pins, WGM11 and WGM10 = 0 (CTC)
	TCCR1B = (1 << WGM12) | (1<<CS12);		// CTC and prescalar = 256
	TIMSK1 = (1 << OCIE1A);					// Interrupt on OCR1A compare match
	
	// Set Midi baud rate
	UBRR0H = (uint8_t) (MIDI_UBRR >> 8);
	UBRR0L = (uint8_t) MIDI_UBRR;
	/* Enable receiver and RxComplete interrupt */
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0);
	/* Frame format: 8data, 1 stop bit (by default) */

	//	
	// Setup SPI for use with DAC
	//
	
	/* Set SCK, MOSI to output, PB4 as device select (active low), all others input */
	DDRB |= (1<<DDB7)|(1<<DDB5)|(1<<DDB4);
	PORTB |= (1<<PB4);
	
	/* Enable SPI, Master, Mode 1, set clock rate fclk/8 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA)|(1<<SPR0);
	
	hal_spi_transmit(DAC_RESET);
	hal_spi_transmit(DAC_RESET);
	hal_spi_transmit(DAC_ENABLE_INT_REF_ALWAYS);
}

void hal_disable_all_segments(void)
{
	PORTD |= 0xF0;
}

void hal_display_digit(uint8_t digit)
{
	PORTA = digit;
}

/*
 * segment - index in range 0-3, 3 is most significant.
 */
void hal_enable_segment(uint8_t segment)
{
	PORTD &= (0xEF << segment) | 0x0F;
}

void hal_gate_off(void)
{
	PORTB |= (1<<PB0);
}

void hal_gate_on(void)
{
	PORTB &= ~(1<<PB0);
}

uint8_t hal_read_keyboard(void)
{
	return (PINB >> 1) & 0x07;
}

void _hal_spi_tx_byte(uint8_t data)
{
	/* Start transmission */
	SPDR = data;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}

void hal_spi_transmit(uint32_t data)
{
	// Assert /CS
	PORTB &= ~(1<<PB4);
	
	_hal_spi_tx_byte((data>>24) & 0xff);
	_hal_spi_tx_byte((data>>16) & 0xff);
	_hal_spi_tx_byte((data>>8) & 0xff);
	_hal_spi_tx_byte(data & 0xff);
	
	PORTB |= (1<<PB4);	
}
/*
 * hal.h
 *
 * Created: 21/08/2014 22:35:31
 *  Author: sjdavies
 */ 


#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

#include <avr/io.h>

#include "dac8568.h"

// CPU clock speed
#define F_CPU 16000000L

// MIDI using USART0
#define MIDI_UART_STATUS			UCSR0A
#define MIDI_UART_DATA				UDR0
#define MIDI_FE						FE0
#define MIDI_DOR					DOR0
#define MIDI_UPE					UPE0
#define MIDI_BAUD					31250
#define MIDI_UBRR					(F_CPU/16/MIDI_BAUD-1)

// Debug over RS-232 using USART1
#define DEBUG_STATUS				UCSR1A

void hal_disable_all_segments(void);
void hal_display_digit(uint8_t);
void hal_enable_segment(uint8_t);
void hal_gate_off(void);
void hal_gate_on(void);
void hal_init(void);

/*
 * Returns raw keyboard bit values.
 * Zeros indicate button is pressed.
 */
uint8_t hal_read_keyboard(void);

/*
 * Sends a 32 bit word over SPI.
 * Blocks until complete.
 */
void hal_spi_transmit(uint32_t);

#endif /* HAL_H_ */

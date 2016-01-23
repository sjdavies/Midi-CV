/*
 * midi.h
 *
 * Created: 29/08/2014 15:59:08
 *  Author: sjdavies
 */ 

#ifndef MIDI_H_
#define MIDI_H_

#include <stdbool.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define MIDI_BAUD				31250
// # of events in midi queue (must be power of two)
#define MIDI_EVENT_COUNT		8

// High order 4 bits of the MIDI status byte
#define MIDI_NOTE_OFF			0x80
#define MIDI_NOTE_ON			0x90
#define MIDI_KEY_AFT			0xA0
#define MIDI_CONTROL_CHG		0xB0
#define MIDI_PROG_CHG			0xC0
#define MIDI_CHANNEL_AFT		0xD0
#define MIDI_PITCH_BEND			0xE0
#define MIDI_SYSTEM_MSG			0xF0

// Extract MIDI channel number
#define MIDI_CHANNEL(b) (b & 0x0F)

// All status bytes have MSB set
#define IS_STATUS(b) (b & 0x80)

// Single byte messages in range F8-FF
#define IS_REAL_TIME(b) ((b & 0xF8) == 0xF8)

// 0xF4 = Undefined (reserved)
// 0xF5 = Undefined (reserved)
// 0xF6 = Tune request
// 0xF7 = End of Exclusive
#define IS_ONE_BYTE_MSG(b) ((b & 0xFC) == 0xF4)

// 0xCn = Program Change
// 0xDn = Channel Pressure (After-touch)
// 0xF1 = MIDI Time Code Quarter Frame
// 0xF3 = Song Select
#define IS_TWO_BYTE_MSG(b) (((b & 0xE0) == 0xC0) || (b == 0xF1) || (b == 0xF3))

// 0x8n = Note off
// 0x9n = Note on
// 0xAn = Polyphonic Key Pressure (Aftertouch)
// 0xBn = Control Change
// 0xEn = Pitch Bend Change
// 0xF2 = Song Position Pointer
#define IS_THREE_BYTE_MSG(b) (((b & 0xC0) == 0x80) || ((b & 0xF0) == 0xE0) || b == 0xF2)
#define IS_NOTE_OFF(b) ((b & 0xF0) == 0x80)
#define IS_NOTE_ON(b) ((b & 0xF0) == 0x90)
#define IS_POLY_AFTERTOUCH(b) ((b & 0xF0) == 0xA0)
#define IS_CONTROL_CHANGE(b) ((b & 0xF0) == 0xB0)
#define IS_PROGRAM_CHANGE(b) ((b & 0xF0) == 0xC0)
#define IS_CHANNEL_AFTERTOUCH(b) ((b & 0xF0) == 0xD0)
#define IS_PITCH_BEND(b) ((b & 0xF0) == 0xE0)
#define IS_CLOCK(b) (b == 0xF8)
#define IS_CLOCK_START(b) (b == 0xFA)
#define IS_CLOCK_CONTINUE(b) (b == 0xFB)
#define IS_CLOCK_STOP(b) (b == 0xFC)
#define IS_ACTIVE_SENSE(b) (b == 0xFE)

typedef struct midi_event midi_event;
struct midi_event {
	uint8_t status;
	uint8_t data[2];
};

midi_event midi_dequeue(void);
void midi_enqueue(uint8_t, uint8_t, uint8_t);
bool midi_isEmpty(void);
void midi_rx(void);

#endif /* MIDI_H_ */
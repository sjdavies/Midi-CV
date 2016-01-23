/*
 * midi.c
 *
 * Created: 29/08/2014 16:02:05
 *  Author: sjdavies
 */ 

#include "hal.h"
#include "midi.h"
#include "param.h"

#define FSM_STATUS_WAIT 0	// default state, waiting for a status byte
#define FSM_DATA1_1 1		// status byte seen, expecting a single data byte
#define FSM_DATA2_1 2
#define FSM_DATA2_2 3

extern int16_t gParams[];

// circular buffer
midi_event gMidiEvent[MIDI_EVENT_COUNT];
// buffer indexes
int gMidiHead = 0;
int gMidiTail = 0;

midi_event midi_dequeue(void)
{
	midi_event event;
	event.status = 0;
	
	cli();
	
	if (!midi_isEmpty()) {
		event.status = gMidiEvent[gMidiTail].status;
		event.data[0] = gMidiEvent[gMidiTail].data[0];
		event.data[1] = gMidiEvent[gMidiTail].data[1];
		gMidiTail = (gMidiTail + 1) % MIDI_EVENT_COUNT;
	}

	sei();
	
	return event;
}

/************************************************************************/
/* Place a singe MIDI message in the queue.                             */
/************************************************************************/
void midi_enqueue(uint8_t status, uint8_t d1, uint8_t d2)
{
	int nextHead = (gMidiHead + 1) % MIDI_EVENT_COUNT;
	
	if (nextHead != gMidiTail) {
		gMidiEvent[gMidiHead].status = status;
		gMidiEvent[gMidiHead].data[0] = d1;
		gMidiEvent[gMidiHead].data[1] = d2;
		gMidiHead = nextHead;
	}
}

bool midi_isEmpty(void)
{
	return gMidiHead == gMidiTail;
}

void midi_rx(void) {
	static uint8_t state = FSM_STATUS_WAIT;
	static uint8_t buff[3];
	static int8_t lastStatus = 0;

	// Get next MIDI byte
	uint8_t status = MIDI_UART_STATUS;
	uint8_t c = MIDI_UART_DATA;

	// Reset state machine if errors are received
	if (status & ((1<<MIDI_FE)|(1<<MIDI_DOR)|(1<<MIDI_UPE))) {
		// one of the 3 error flags set
		state = FSM_STATUS_WAIT;
		lastStatus = 0;
		return;
	}

	if (IS_REAL_TIME(c)) {
		// Real time messages can occur anywhere in the MIDI data stream,
		// including in the 'middle' of the current command.
		midi_enqueue(c, 0, 0);
	} else {
		switch (state) {
		case FSM_STATUS_WAIT:
			if (IS_STATUS(c)) {
				buff[0] = c;
				lastStatus = c;
				if (IS_ONE_BYTE_MSG(c)) {
					midi_enqueue(c, 0, 0);
				} else if (IS_TWO_BYTE_MSG(c)) {
					state = FSM_DATA1_1;
				} else if (IS_THREE_BYTE_MSG(c)) {
					state = FSM_DATA2_1;
				} else {
					// unknown format
					lastStatus = 0;
				}
			} else {
				// must be data
				if (IS_ONE_BYTE_MSG(lastStatus)) {
					// something wrong, no data byte in one byte msg
					lastStatus = 0;
				} else if (IS_TWO_BYTE_MSG(lastStatus)) {
					buff[0] = lastStatus;
					midi_enqueue(buff[0], c, 0);
				} else if (IS_THREE_BYTE_MSG(lastStatus)) {
					buff[0] = lastStatus;
					buff[1] = c;
					state = FSM_DATA2_2;
				} else {
					// unknown format
					lastStatus = 0;
				}
			}
			break;
		case FSM_DATA1_1:
			if (IS_STATUS(c)) {
				buff[0] = c;
				lastStatus = c;
				if (IS_ONE_BYTE_MSG(c)) {
					midi_enqueue(c, 0, 0);
					state = FSM_STATUS_WAIT;
				} else if (IS_TWO_BYTE_MSG(c)) {
					// remain in this state
				} else if (IS_THREE_BYTE_MSG(c)) {
					state = FSM_DATA2_1;
				} else {
					// unknown format
					lastStatus = 0;
					state = FSM_STATUS_WAIT;
				}
			} else {
				// must be data
				midi_enqueue(buff[0], c, 0);
				state = FSM_STATUS_WAIT;
			}
			break;
		case FSM_DATA2_1:
			if (IS_STATUS(c)) {
				buff[0] = c;
				lastStatus = c;
				if (IS_ONE_BYTE_MSG(c)) {
					midi_enqueue(c, 0, 0);
					state = FSM_STATUS_WAIT;
				} else if (IS_TWO_BYTE_MSG(c)) {
					state = FSM_DATA1_1;
				} else if (IS_THREE_BYTE_MSG(c)) {
					// remain in this state
				} else {
					// unknown format
					lastStatus = 0;
					state = FSM_STATUS_WAIT;
				}
			} else {
				// must be data
				buff[1] = c;
				state = FSM_DATA2_2;
			}
			break;
		case FSM_DATA2_2:
			if (IS_STATUS(c)) {
				buff[0] = c;
				lastStatus = c;
				if (IS_ONE_BYTE_MSG(c)) {
					midi_enqueue(c, 0, 0);
					state = FSM_STATUS_WAIT;
				} else if (IS_TWO_BYTE_MSG(c)) {
					state = FSM_DATA1_1;
				} else if (IS_THREE_BYTE_MSG(c)) {
					state = FSM_DATA2_1;
				} else {
					// unknown format
					lastStatus = 0;
					state = FSM_STATUS_WAIT;
				}
			} else {
				// must be data
				midi_enqueue(buff[0], buff[1], c);
				state = FSM_STATUS_WAIT;
			}
			break;
		default:
			state = FSM_STATUS_WAIT;
			lastStatus = 0;
		}
	}
}

/*
 * Midi_CV.c
 *
 * Created: 21/08/2014 22:05:34
 *  Author: sjdavies
 */ 

#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "hal.h"
#include <util/delay.h>

#include "dac8568.h"
#include "SevenSeg.h"
#include "keyboard.h"
#include "param.h"
#include "midi.h"

//
// Menu finite state machine
//
#define FSM_PARAM		0
#define FSM_VALUE		1

int16_t gParams[PARAM_COUNT];
uint8_t gCurrentNote = 0xFF;			// Midi note number

void displayController(int16_t);
void displayParam(uint8_t);
void factory_reset(void);
void handleSystemMsg(midi_event *);
void initialise_params(void);
void launch_midi_analyser(void);
void paramValueDec(uint8_t);
void paramValueInc(uint8_t);
void update_param(uint8_t);

int main(void)
{
	uint8_t state = FSM_PARAM;
	uint8_t param = 0;

	initialise_params();		
	hal_init();
	
	sei();
	
	// Check for buttons down on startup
	uint8_t buttons = hal_read_keyboard();
	if (buttons < 7) {
		switch (buttons) {
			case 0:
				// all buttons down
				factory_reset();
				seg7_display_char(2, 'F');
				seg7_display_char(1, 'd');
				seg7_display_char(0, ' ');
				while (1) {}
				break;
			case 6:
				// SEL button only
				launch_midi_analyser();
				break;
			default:
				break;
		}
	}
	
	seg7_display_dp(2);
	seg7_display_char(2, 'P');
	seg7_display_dec(gParamData[param].id, 2);
	
    while(1) {
		if (!kbd_isEmpty()) {
			uint8_t key = kbd_dequeue();
			
			if (KBD_IS_BUTTON_DOWN(key) && KBD_IS_BUTTON0(key)) {
				// Select button
				if (state == FSM_PARAM) {
					state = FSM_VALUE;
					seg7_clear_dp(2);
					seg7_display_dp(1);
					seg7_display_char(2, ' ');
					displayParam(param);
				} else if (state == FSM_VALUE) {
					state = FSM_PARAM;
					update_param(param);
					seg7_clear_dp(1);
					seg7_display_dp(2);
					seg7_display_char(2, 'P');
					seg7_display_dec(gParamData[param].id, 2);
				}
			} else if (KBD_IS_BUTTON_DOWN(key) && KBD_IS_BUTTON1(key)) {
				// INC button
				if (state == FSM_PARAM) {
					param = (param == (PARAM_COUNT - 1)) ? 0 : (param + 1);
					seg7_display_dec(gParamData[param].id, 2);
				} else {
					paramValueInc(param);
				}
			} else if (KBD_IS_BUTTON_DOWN(key) && KBD_IS_BUTTON2(key)) {
				// DEC button
				if (state == FSM_PARAM) {
					param = (param == 0) ? (PARAM_COUNT - 1) : (param - 1);
					seg7_display_dec(gParamData[param].id, 2);
				} else {
					paramValueDec(param);
				}
			}			
		}
		
		if (!midi_isEmpty()) {
			midi_event event = midi_dequeue();
			uint8_t channel = event.status & 0x0F;
			
			switch (event.status & 0xF0) {
				case MIDI_SYSTEM_MSG:
					handleSystemMsg(&event);
					break;
				case MIDI_NOTE_ON:
					if (channel == gParams[PARAM_CHANNEL]) {
						if ((event.data[1] == 0) && (event.data[0] == gCurrentNote)) {
							// zero velocity, treat as note off
							gCurrentNote = 0xff;	
							hal_gate_off();	
						} else {
							// need to consider new, high or lower
							gCurrentNote = event.data[0];
							hal_gate_on();
						}
					}
					break;
				case MIDI_NOTE_OFF:
					if (channel == gParams[PARAM_CHANNEL]) {
						if (event.data[0] == gCurrentNote) {
							gCurrentNote = 0xff;
							hal_gate_off();
						}
					}
					break;
				case MIDI_PITCH_BEND:
					break;
				case MIDI_CONTROL_CHG:
					break;
				case MIDI_PROG_CHG:
					break;
				case MIDI_KEY_AFT:
					break;
				case MIDI_CHANNEL_AFT:
					break;
				default:
					break;
			}
		}
    }
}

/************************************************************************/
/* Fires every 50ms.                                                    */
/************************************************************************/
ISR(TIMER1_COMPA_vect)
{
	seg7_display_next();
	kbd_scan();
}

ISR(USART0_RX_vect)
{
	midi_rx();
}

/*
 * Several parameters specify MIDI controller numbers.
 * Display the possible values.
 */
void displayController(int16_t value)
{
	switch (value) {
	case PARAM_CONTROL_OFF:
		seg7_display_char(2, 'O');
		seg7_display_char(1, 'F');
		seg7_display_char(0, 'F');
		break;
	case PARAM_CONTROL_PB:
		seg7_display_char(2, 'P');
		seg7_display_char(1, 'b');
		seg7_display_char(0, 'd');
		break;
	case PARAM_CONTROL_VEL:
		seg7_display_char(2, 'V');
		seg7_display_char(1, 'E');
		seg7_display_char(0, 'L');
		break;
	case PARAM_CONTROL_AFT:
		seg7_display_char(2, 'A');
		seg7_display_char(1, 'F');
		seg7_display_char(0, 't');
		break;
	default:
		seg7_display_dec(value, 3);
	}
}

void displayParam(uint8_t param) {
	switch (param) {
	case PARAM_CHANNEL:			// P01 - MIDI channel
		seg7_display_char(2, ' ');
		seg7_display_dec(gParams[param] + 1, 2);
		break;
	case PARAM_RETRIGGER:		// P02 - Gate retrigger time
		if (gParams[param] == PARAM_RETRIG_OFF) {
			seg7_display_char(2, 'O');
			seg7_display_char(1, 'F');
			seg7_display_char(0, 'F');
		} else {
			seg7_display_char(2, ' ');
			seg7_display_dec(gParams[param], 2);
		}
		break;
	case PARAM_NOTEP:		// P03 - Note priority
		switch (gParams[param]) {
		case PARAM_NOTEP_NEW:
			seg7_display_char(2, ' ');
			seg7_display_char(1, 'n');
			seg7_display_char(0, 'n');
			break;
		case PARAM_NOTEP_LOW:
			seg7_display_char(2, ' ');
			seg7_display_char(1, 'l');
			seg7_display_char(0, 'o');
			break;
		case PARAM_NOTEP_HIGH:
			seg7_display_char(2, ' ');
			seg7_display_char(1, 'h');
			seg7_display_char(0, 'i');
			break;
		}
		break;
	case PARAM_PBRANGE:		// Pitch Bend Range
		seg7_display_char(2, ' ');
		seg7_display_dec(gParams[param], 2);
		break;
	case PARAM_PORTA_CONTROL:
		switch (gParams[param]) {
		case PARAM_PORTAMENTO_ON:
			seg7_display_char(2, 'O');
			seg7_display_char(1, 'N');
			seg7_display_char(0, ' ');
			break;
		case PARAM_PORTAMENTO_OFF:
			seg7_display_char(2, 'O');
			seg7_display_char(1, 'F');
			seg7_display_char(0, 'F');
			break;
		case PARAM_PORTAMENTO_AUTO:
			seg7_display_char(2, 'A');
			seg7_display_char(1, 'U');
			seg7_display_char(0, ' ');
			break;
		default:
			seg7_display_dec(gParams[param], 3);
		}
		break;
	case PARAM_PORTA_TIME:		// Portamento time
	case PARAM_LFO_MIN:			// LFO to CV minimum value
	case PARAM_LFO_MAX:			// LFO to CV maximum value
	case PARAM_LFO_RESET:		// LFO to CV reset value
	case PARAM_AUX1_RESET:		// AUX1 reset value
	case PARAM_AUX1_KEY_SCALE:	// Key scale to AUX1
	case PARAM_AUX1_LFO_MIN:	// LFO to AUX1 minimum value
	case PARAM_AUX1_LFO_MAX:	// LFO to AUX1 maximum value
	case PARAM_AUX1_LFO_RESET:	// LFO to AUX1 reset value
	case PARAM_LFO_SYNC_START:	// LFO sync start point
	case PARAM_AUX2_RESET:		// AUX2 reset value
	case PARAM_AUX3_MIN:		// AUX3 minimum value
	case PARAM_AUX3_MAX:		// AUX3 maximum value
	case PARAM_AUX3_RESET:		// AUX3 reset value
	case PARAM_AUX4_MIN:		// AUX4 minimum value
	case PARAM_AUX4_MAX:		// AUX4 maximum value
	case PARAM_AUX4_RESET:		// AUX4 reset value
		seg7_display_dec(gParams[param], 3);
		break;
	case PARAM_PORTA_TYPE:		// Portamento type
		switch (gParams[param]) {
		case PARAM_PORTA_FIXED_RATE:
			seg7_display_char(2, 'F');
			seg7_display_char(1, 'r');
			seg7_display_char(0, ' ');
			break;
		case PARAM_PORTA_FIXED_TIME:
			seg7_display_char(2, 'F');
			seg7_display_char(1, 't');
			seg7_display_char(0, ' ');
		}
		break;
	case PARAM_LFO_CONTROL:		// LFO to CV controller number
	case PARAM_AUX1_CONTROL:	// AUX1 controller number
	case PARAM_AUX1_LFO_CONTROL:	// LFO to AUX1 controller
	case PARAM_AUX2_CONTROL:	// AUX2 controller number
	case PARAM_AUX3_CONTROL:	// AUX3 controller number
	case PARAM_AUX4_CONTROL:	// AUX4 controller number
		displayController(gParams[param]);
		break;
	case PARAM_COARSE_TUNE:
	case PARAM_FINE_TUNE:
	case PARAM_SCALE:
	case PARAM_AUX1_MIN:		// AUX1 minimum value
	case PARAM_AUX1_MAX:		// AUX1 maximum value
	case PARAM_AUX2_MIN:		// AUX2 minimum value
	case PARAM_AUX2_MAX:		// AUX2 maximum value
		// TODO display negative values
		break;
	case PARAM_CV_ALGORITHM:	// CV - Hz/V - 1.2V/oct select
		// TODO
		break;
	case PARAM_GATE_TYPE:		// Gate type select
		// TODO
		break;
	case PARAM_LFO_RATE:		// LFO rate/speed
		// TODO
		break;
	case PARAM_LFO_WAVESHAPE:	// LFO waveshape
		// TODO
		break;
	case PARAM_LFO_MIDI_SYNC:	// LFO MIDI sync
		// TODO
		break;
	case PARAM_LFO_KEYON_RESET:	// Key-on resets LFO wave
		// TODO
		break;
	}
} 

/*
 * Revert all parameter settings to the factory defaults and store persistently.
 */
void factory_reset(void)
{
	for (int i = 0; i < PARAM_COUNT; i++) {
		gParams[i] = gParamData[i].init;
	}
	
	eeprom_update_block(gParams, 0, sizeof(gParams));
}

void handleSystemMsg(midi_event * pEvent)
{
	
}

/*
 * Load all parameters from EEPROM.
 * If first time, then reset to factory defaults and update eeprom.
 */
void initialise_params(void) {
	
	eeprom_read_block(gParams, 0, sizeof(gParams));
	
	// Uninitialised eeprom stores all FF's
	if (gParams[0] == 0xFFFF) {
		factory_reset();
	}
}

enum ana_mode {
	RECEIVE,
	PROG_CHG,
	NOTE,
	VELOCITY,
	CTRL_NUM,
	CTRL_VAL
	};

void launch_mode_display(enum ana_mode mode)
{
	seg7_display_char(2, ' ');
	
	switch (mode) {
	case RECEIVE:
		seg7_display_char(1, 'r');
		seg7_display_char(0, 'C');
		break;
	case PROG_CHG:
		seg7_display_char(1, 'P');
		seg7_display_char(0, 'C');
		break;
	case NOTE:
		seg7_display_char(1, 'n');
		seg7_display_char(0, 't');
		break;
	case VELOCITY:
		seg7_display_char(1, 'n');
		seg7_display_char(0, 'v');
		break;
	case CTRL_NUM:
		seg7_display_char(1, 'C');
		seg7_display_char(0, 'n');
		break;
	case CTRL_VAL:
		seg7_display_char(1, 'C');
		seg7_display_char(0, 'v');
		break;
	default:
		seg7_display_char(1, '-');
		seg7_display_char(0, '-');
		break;
	}
}

void launch_midi_analyser(void) {
	enum ana_mode mode = CTRL_VAL;
	
	while (1) {
		launch_mode_display(mode);
		
		if (!kbd_isEmpty()) {
			
		}
		
		if (!midi_isEmpty()) {
			midi_event event = midi_dequeue();
			uint8_t channel = event.status & 0x0F;
			
			// Ignore active sense bytes
			if (!IS_ACTIVE_SENSE(event.status)) {
				seg7_display_dp(2);
			
				switch (mode) {
					case RECEIVE:
						if (IS_NOTE_ON(event.status)) {
							seg7_display_raw(2, 0xFB);
							seg7_display_dec(channel + 1, 2);
							_delay_ms(200);
						} else if (IS_NOTE_OFF(event.status)) {
							seg7_display_raw(2, 0x04);
							seg7_display_dec(channel + 1, 2);
							_delay_ms(200);
						} else if (IS_CLOCK(event.status)) {
							seg7_display_raw(2, 0xF0);
						} else if (IS_CLOCK_START(event.status)) {
							seg7_display_raw(2, 0xBF);
						} else if (IS_CLOCK_STOP(event.status)) {
							seg7_display_raw(2, 0xF7);
						} else if (IS_CLOCK_CONTINUE(event.status)) {
							seg7_display_raw(2, 0xDF);
						}
						break;
					case PROG_CHG:
						if (IS_PROGRAM_CHANGE(event.status)) {
							seg7_display_dec(event.data[0] + 1, 3);
							_delay_ms(200);
						}
						break;
					case NOTE:
						if (IS_NOTE_ON(event.status) || IS_NOTE_OFF(event.status)) {
							seg7_display_dec(event.data[0], 3);
							_delay_ms(200);
						}
						break;
					case VELOCITY:
						if (IS_NOTE_ON(event.status) || IS_NOTE_OFF(event.status)) {
							seg7_display_dec(event.data[1], 3);
							_delay_ms(200);
						}
						break;
					case CTRL_NUM:
						if (IS_CONTROL_CHANGE(event.status)) {
							seg7_display_dec(event.data[0], 3);
							_delay_ms(200);
						} else if (IS_PITCH_BEND(event.status)) {
							seg7_display_char(1, 'p');
							seg7_display_char(0, 'b');
						} else if (IS_POLY_AFTERTOUCH(event.status) || IS_CHANNEL_AFTERTOUCH(event.status)) {
							seg7_display_char(1, 'a');
							seg7_display_char(0, 'f');
						}
						break;
					case CTRL_VAL:
						if (IS_CONTROL_CHANGE(event.status)) {
							seg7_display_dec(event.data[1], 3);
							_delay_ms(200);
						}
						break;
					default:
						break;
				}
			
				_delay_ms(100);
				seg7_clear_dp(2);
			}
		}
	}	
}

void paramValueDec(uint8_t param) {
	int16_t current = gParams[param];

	if (current == gParamData[param].min) {
		gParams[param] = gParamData[param].max;
	} else {
		gParams[param] = current - 1;
	}
	
	displayParam(param);
}

void paramValueInc(uint8_t param) {
	int16_t current = gParams[param];

	if (current == gParamData[param].max) {
		gParams[param] = gParamData[param].min;
	} else {
		gParams[param] = current + 1;
	}
	
	displayParam(param);
}

/*
 * Stores a possibly updated parameter value in eeprom.
 */
void update_param(uint8_t param) {
	eeprom_update_word((uint16_t *)(param * 2), (uint16_t)gParams[param]);
}
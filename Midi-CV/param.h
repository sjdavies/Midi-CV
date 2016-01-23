/*
 * param.h
 *
 * Parameter values for controlling the MIDI-CV device.
 *
 * Created: 25 August, 2014
 *  Author: Stephen Davies
 */ 

#ifndef PARAM_H_
#define PARAM_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#define PARAM_COUNT				42

#define PARAM_CHANNEL			0
#define PARAM_RETRIGGER			1
#define PARAM_NOTEP				2
#define PARAM_PBRANGE			3
#define PARAM_PORTA_CONTROL		4
#define PARAM_PORTA_TIME		5
#define PARAM_PORTA_TYPE		6
#define PARAM_LFO_CONTROL		7
#define PARAM_LFO_MIN			8
#define PARAM_LFO_MAX			9
#define PARAM_LFO_RESET			10
#define PARAM_COARSE_TUNE		11
#define PARAM_FINE_TUNE			12
#define PARAM_SCALE				13
#define PARAM_CV_ALGORITHM		14
#define PARAM_GATE_TYPE			15
#define PARAM_AUX1_CONTROL		16
#define PARAM_AUX1_MIN			17
#define PARAM_AUX1_MAX			18
#define PARAM_AUX1_RESET		19
#define PARAM_AUX1_KEY_SCALE	20
#define PARAM_AUX1_LFO_CONTROL	21
#define PARAM_AUX1_LFO_MIN		22
#define PARAM_AUX1_LFO_MAX		23
#define PARAM_AUX1_LFO_RESET	24
#define PARAM_LFO_RATE			25
#define PARAM_LFO_WAVESHAPE		26
#define PARAM_LFO_MIDI_SYNC		27
#define PARAM_LFO_SYNC_START	28
#define PARAM_LFO_KEYON_RESET	29
#define PARAM_AUX2_CONTROL		30
#define PARAM_AUX2_MIN			31
#define PARAM_AUX2_MAX			32
#define PARAM_AUX2_RESET		33
#define PARAM_AUX3_CONTROL		34
#define PARAM_AUX3_MIN			35
#define PARAM_AUX3_MAX			36
#define PARAM_AUX3_RESET		37
#define PARAM_AUX4_CONTROL		38
#define PARAM_AUX4_MIN			39
#define PARAM_AUX4_MAX			40
#define PARAM_AUX4_RESET		41

// Gate retrigger time 0-25, 0 == off
#define PARAM_RETRIG_OFF		0

// Note priority 0-2
#define PARAM_NOTEP_NEW			0
#define PARAM_NOTEP_LOW			1
#define PARAM_NOTEP_HIGH		2

// Portamento controller -3 to 119
#define PARAM_PORTAMENTO_ON 	(-3)
#define	PARAM_PORTAMENTO_OFF	(-2)
#define PARAM_PORTAMENTO_AUTO	(-1)

// Portamento type 0-1
#define PARAM_PORTA_FIXED_RATE	0
#define PARAM_PORTA_FIXED_TIME	1

// Most controllers use these settings
#define PARAM_CONTROL_OFF		(-4)
#define PARAM_CONTROL_PB		(-3)
#define PARAM_CONTROL_VEL		(-2)
#define PARAM_CONTROL_AFT		(-1)

// CV Algorithm
#define PARAM_CV_VOLT_OCT		0
#define PARAM_CV_VOLT_HERTZ		1
#define PARAM_CV_1V2_OCT		2

// Gate type
#define PARAM_GATE_VTRIG_5		0
#define PARAM_GATE_VTRIG_10		1
#define PARAM_GATE_VTRIG_12		2
#define PARAM_GATE_STRIG_5		3
#define PARAM_GATE_STRIG_10		4
#define PARAM_GATE_STRIG_12		5

//
// Declare format for a single control parameter.
//
typedef struct paramEntry paramEntry;
struct paramEntry {
	uint8_t		id;			// Parameter number shown in display
	int16_t		min;		// minimum value (inclusive)
	int16_t		max;		// maximum value (inclusive)
	int16_t		init;		// default value
};

//
// Global parameter data table
//
extern const paramEntry gParamData[];

#endif /* PARAM_H_ */

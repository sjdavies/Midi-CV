/*
 * param.c
 *
 * Created: 25/08/2014 17:57:09
 *  Author: sjdavies
 */ 

#include "param.h"

const paramEntry gParamData[PARAM_COUNT] = {
	{  1, 0, 15, 0 },		// MIDI channel
	{  2, 0, 25, 5 },		// gate retrigger time (x200ns)
	{  3, 0, 2, 0 },		// note priority
	{  4, 0, 12, 2 },		// pitch bend range (semitones)
	{  5, -3, 119, 65 },	// portamento controller #
	{  6, 1, 127, 98 },		// portamento time/rate
	{  7, 0, 1, 0 },		// portamento type
	{  8, -4, 119, 1 },		// LFO to CV controller #
	{  9, 0, 127, 0 },		// LFO to CV min value
	{ 10, 0, 127, 50 },		// LFO to CV max value
	{ 11, 0, 127, 0 },		// LFO to CV reset value
	{ 12, -24, 24, 0 },		// Coarse tune / transpose
	{ 13, -127, 127, 0 },	// Fine tune
	{ 14, -127, 127, 0 },	// Scale
	{ 15, 0, 2, 0 },		// CV - Hz/V - 1.2CV
	{ 16, 0, 5, 0 },		// Gate / S-trig levels
// AUX 1
	{ 20, -4, 119, 16 },	// AUX 1 controller #
	{ 21, -27, 100, 0 },	// AUX 1 min value
	{ 22, -27, 100, 100 },	// AUX 1 max value
	{ 23, 0, 127, 0 },		// AUX 1 reset value	
	{ 24, 0, 127, 0 },		// Key scale to AUX 1
	{ 25, -4, 119, 17 },	// LFO to AUX 1 controller #
	{ 26, 0, 127, 0 },		// LFO to AUX 1 min value
	{ 27, 0, 127, 64 },		// LFO to AUx 1 max value
	{ 28, 0, 127, 0 },		// LFO to AUX 1 reset value
// LFO Control
	{ 30, 0, 191, 116 },	// LFO rate / speed
	{ 31, 0, 8, 0 },		// LFO wave shape
	{ 32, 0, 96, 0 },		// LFO MIDI sync
	{ 33, 0, 255, 0 },		// LFO sync start point
	{ 34, 0, 1, 0 },		// key-on resets LFO wave
// AUX 2
	{ 40, -4, 119, 7 },		// AUX 2 controller #
	{ 41, -27, 100, 0 },	// AUX 2 min value
	{ 42, -27, 100, 100 },	// AUX 2 max value
	{ 43, 0, 127, 127},		// AUX 2 reset value
// AUX 3
	{ 50, -4, 119, -2 },	// AUX 3 controller #
	{ 51, 0, 100, 0 },		// AUX 3 min value
	{ 52, 0, 100, 100 },	// AUX 3 max value
	{ 53, 0, 127, 0 },		// AUX 3 reset value
// AUX 4
	{ 60, -4, 119, -1 },	// AUX 4 controller #
	{ 61, 0, 100, 0 },		// AUX 4 min value
	{ 62, 0, 100, 100 },	// AUX 4 max value
	{ 63, 0, 127, 0 }		// AUX 4 reset value
		
};

const int8_t paramCount = sizeof(gParamData) / sizeof(paramEntry);

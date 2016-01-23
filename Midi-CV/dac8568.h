/*
 * dac8568.h
 *
 * Constants and macros for use with Texas Instruments
 * DAC chips:
 *   DAC7568
 *   DAC8168
 *   DAC8568
 *
 * Created: 16 April, 2015
 *  Author: Stephen Davies
 */
 
#ifndef DAC8568_H
#define DAC8568_H

#include <stdint.h>

//
// Commands
//
#define DAC_RESET							0x07000000	// Software reset
#define DAC_ENABLE_INT_REF_STATIC			0x08000001
#define DAC_DISABLE_INT_REF_STATIC			0x08000000
#define DAC_ENABLE_INT_REF_FLEX				0x09080000
#define DAC_DISABLE_INT_REF_FLEX			0x090C0000
#define DAC_ENABLE_INT_REF_ALWAYS			0x090A0000
#define DAC_WRITE_AND_UPDATE_REG			0x03000000
#define DAC_WRITE_REG_UPDATE_ALL			0x02000000

#define DAC_CHANNEL_A						0
#define DAC_CHANNEL_B						1
#define DAC_CHANNEL_C						2
#define DAC_CHANNEL_D						3
#define DAC_CHANNEL_E						4
#define DAC_CHANNEL_F						5
#define DAC_CHANNEL_G						6
#define DAC_CHANNEL_H						7
#define DAC_CHANNEL_ALL						15			// broadcast

/*
 * Merge all three bit fields to create a single DAC command word.
 */
inline uint32_t dac_command(uint32_t cmd, uint32_t channel, uint32_t data) {
	return cmd | ((channel & 0x0f)<<20) | ((data & 0xffff)<<4);
}

#endif // DAC8568_H

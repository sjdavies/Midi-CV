/*
 * keyboard.h
 *
 * Created: 24/08/2014 17:20:29
 *  Author: sjdavies
 */ 


#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>

#define KBD_EVENT_COUNT		8			// size of keyboard event queue, must be power of two
#define KBD_BUTTON_COUNT	3			// number of buttons

#define KBD_KEY_DOWN		0
#define KBD_KEY_UP			0x80

#define KBD_IS_BUTTON_DOWN(k) ((k & 0x80) == 0)
#define KBD_IS_BUTTON0(k) ((k & 0x7F) == 0)
#define KBD_IS_BUTTON1(k) ((k & 0x7F) == 1)
#define KBD_IS_BUTTON2(k) ((k & 0x7F) == 2)

typedef struct kbd_event kbd_event;
struct kbd_event {
	// bit 7 = 0 (down), 1 (up)
	// bits 1-0 = key
	uint8_t data;
};

uint8_t kbd_dequeue(void);
void kbd_enqueue(uint8_t);
bool kbd_isEmpty(void);
void kbd_scan(void);

#endif /* KEYBOARD_H_ */
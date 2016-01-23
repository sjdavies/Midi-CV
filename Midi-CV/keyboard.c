/*
 * keyboard.c
 *
 * Created: 24/08/2014 17:22:44
 *  Author: sjdavies
 */ 

#include "hal.h"
#include "keyboard.h"

// circular buffer
kbd_event gKeyEvent[KBD_EVENT_COUNT];

// buffer indexes
volatile int gKeyHead = 0;
volatile int gKeyTail = 0;

uint8_t kbd_dequeue(void)
{
	if (kbd_isEmpty()) {
		return 0x04;
	} else {
		uint8_t key = gKeyEvent[gKeyTail].data;
		gKeyTail = (gKeyTail + 1) % KBD_EVENT_COUNT;
		return key;
	}
}

void kbd_enqueue(uint8_t data)
{
	int nextHead = (gKeyHead + 1) % KBD_EVENT_COUNT;
	
	if (nextHead != gKeyTail) {
		gKeyEvent[gKeyHead].data = data;
		gKeyHead = nextHead;
	}
}

bool kbd_isEmpty(void)
{
	return gKeyHead == gKeyTail;
}

/*
 * Check the buttons and keep a running tally of each button state.
 */
void kbd_scan(void)
{
	static uint8_t buttons[KBD_BUTTON_COUNT] = { 0xFF, 0xFF, 0xFF };
		
	uint8_t buttonScan = hal_read_keyboard();
	
	for (int i = 0; i < KBD_BUTTON_COUNT; i++) {
		buttons[i] = (buttons[i] << 1) | (buttonScan & 1);
		buttonScan >>= 1;
		
		if (buttons[i] == 0x80) {
			// button down
			kbd_enqueue(KBD_KEY_DOWN | i);
		} else if (buttons[i] == 0x7F) {
			// button up
			kbd_enqueue(KBD_KEY_UP | i);
		}
	}
}

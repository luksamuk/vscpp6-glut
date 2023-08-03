#ifndef KEYBOARD_HPP_INCLUDED
#define KEYBOARD_HPP_INCLUDED

#define BTN_UP      0x0
#define BTN_DOWN    0x1
#define BTN_LEFT    0x2
#define BTN_RIGHT   0x3
#define BTN_START   0x4
#define BTN_ACTION1 0x5
#define BTN_ACTION2 0x6

struct KeyboardState
{
	bool btns[7];
};

void kbdInit(void);
void kbdUpdateButton(unsigned int button, bool state);
bool kbdPressing(unsigned int button);
bool kbdPressed(unsigned int button);

#endif // KEYBOARD_HPP_INCLUDED
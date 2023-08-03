#include "keyboard.hpp"
#include <cstring>

static KeyboardState current_state;
static KeyboardState prev_state;

void
kbdInit(void)
{
	memset(current_state.btns, 0, 7 * sizeof(bool));
	memset(prev_state.btns, 0, 7 * sizeof(bool));
}

void
kbdUpdateButton(unsigned int button, bool state)
{
	prev_state.btns[button] = current_state.btns[button];
	current_state.btns[button] = state;
}

bool
kbdPressing(unsigned int button)
{
	return current_state.btns[button];
}

bool
kbdPressed(unsigned int button)
{
	return current_state.btns[button] && !prev_state.btns[button];
}

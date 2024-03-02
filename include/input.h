#ifndef INPUT_H_
#define INPUT_H_

#include <cpu_6502_emulator.h>

wchar_t* wcscasechr(wchar_t *string,wchar_t character);

void input_mouse_over(App *app);

void input_mouse_button_down(App *app);

Vector2i set_pos_in_input(App *app);

void open_input(App *app,Input *input,uint32_t value);

void close_input(App *app);

void input_events(App *app);

#endif
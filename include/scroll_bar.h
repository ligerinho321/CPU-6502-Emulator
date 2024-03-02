#ifndef SCROLL_BAR_H_
#define SCROLL_BAR_H_

#include <cpu_6502_emulator.h>

#define SCROLL_BAR_SIZE 10
#define SCROLL_BAR_MIN_SIZE 50

float GetScrollBarHeight(Scroll_Bar scroll_bar);
float GetScrollBarWidth(Scroll_Bar scroll_bar);

void SetVerticalOffsetScrollBar(Scroll_Bar *scroll_bar);
void SetHorizontalOffsetScrollBar(Scroll_Bar *scroll_bar);

void AjustVerticalScrollBar(Scroll_Bar *scroll_bar);
void AjustHorizontalScrollBar(Scroll_Bar *scroll_bar);

void SetYScrollBar(Scroll_Bar *scroll_bar,float offset);
void SetXScrollBar(Scroll_Bar *scroll_bar,float offset);

void HandleVerticalScrollBar(Scroll_Bar *scroll_bar,Mouse mouse);
void HandleHorizontalScrollBar(Scroll_Bar *scroll_bar,Mouse mouse);

void DrawScrollBar(Scroll_Bar *scroll_bar,Renderer *renderer);

#endif
#ifndef BREAKPOINTS_H_
#define BREAKPOINTS_H_

#include <cpu_6502_emulator.h>
#include <utils.h>
#include <scroll_bar.h>
#include <input.h>

void CheckBreakpoints(Breakpoints *breakpoints);

void BreakpointsAjustVerticalScrollBarOffset(Breakpoints *breakpoints);
void BreakpointsConfigVerticalScrollBar(Breakpoints *breakpoints);

void ConfigBreakpointsRect(Breakpoints *breakpoints);

void PushBreakpoint(Breakpoints *breakpoints);
void RemoveBreakpoint(Breakpoints *breakpoints);

void BreakpointsInit(Breakpoints *breakpoints,App *app);
void BreakpointsConfigRect(Breakpoints *breakpoints);
void BreakpointsDraw(Breakpoints *breakpoints);
void BreakpointsEvent(Breakpoints *breakpoints);
void BreakpointsFree(Breakpoints *breakpoints);

#endif
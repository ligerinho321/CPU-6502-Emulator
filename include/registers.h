#ifndef REGISTERS_H_
#define REGISTERS_H_

#include <cpu_6502_emulator.h>
#include <utils.h>

void RegistersInit(Registers *registers,App *app);
void RegistersConfiRect(Registers *registers);
void RegistersDraw(Registers *registers);
void RegistersEvent(Registers *registers);
void RegistersFree(Registers *registers);

#endif
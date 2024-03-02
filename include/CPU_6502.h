#ifndef CPU_6502_H_
#define CPU_6502_H_

#include <string.h>

#define MAX_UINT8 0x00100
#define MAX_UINT16 0x10000

#define TABLE_SIZE MAX_UINT8

#define MEMORY_SIZE MAX_UINT16

#define ZERO_PAGE_START 0x0000
#define ZERO_PAGE_END 0x00FF

#define STACK_START 0x0100
#define STACK_END 0x01FF
#define STACK_LENGTH 0x00FF

#define CPU_6502_BIT_7 0x0080
#define CPU_6502_BIT_6 0x0040
#define CPU_6502_BIT_5 0x0020
#define CPU_6502_BIT_4 0x0010
#define CPU_6502_BIT_3 0x0008
#define CPU_6502_BIT_2 0x0004
#define CPU_6502_BIT_1 0x0002
#define CPU_6502_BIT_0 0x0001

#define CPU_6502_ZERO 0x0000
#define CPU_6502_BYTE 0x00FF

//Sytem Vectors
//$FFFA, $FFFB ... NMI (Non-Maskable Interrupt) vector, 16-bit (LB, HB)
#define NMI_ADDRESS 0xFFFA
//$FFFC, $FFFD ... RES (Reset) vector, 16-bit (LB, HB)
#define RES_ADDRESS 0xFFFC
//$FFFE, $FFFF ... IRQ (Interrupt Request) vector, 16-bit (LB, HB)
#define IRQ_ADDRESS 0xFFFE

//Stack Register Flags (bit 7 to bit 0)
#define CPU_6502_FLAG_N CPU_6502_BIT_7 // bit7 ... Negative (N)
#define CPU_6502_FLAG_V CPU_6502_BIT_6 // bit6 ... Overflow (V)
#define CPU_6502_FLAG_U CPU_6502_BIT_5 // bit5 ... ignored
#define CPU_6502_FLAG_B CPU_6502_BIT_4 // bit4 ... Break (B)
#define CPU_6502_FLAG_D CPU_6502_BIT_3 // bit3 ... Decimal (use BCD for arithmetics) (D)
#define CPU_6502_FLAG_I CPU_6502_BIT_2 // bit2 ... Interrupt (IRQ disable) (I)
#define CPU_6502_FLAG_Z CPU_6502_BIT_1 // bit1 ... Zero (Z)
#define CPU_6502_FLAG_C CPU_6502_BIT_0 // bit0 ... Carry (C)

#define READ_8_BITS(cpu,address)  ((cpu)->memory[address])

#define READ_16_BITS(cpu,address) (((CPU_6502_uint16_t)(cpu)->memory[(address) + 1] << 8) | ((CPU_6502_uint16_t)(cpu)->memory[address]))

#define WRITE_8_BITS(cpu,address,value) ((cpu)->memory[address] = value)

#define WRITE_16_BITS(cpu,address,value)              \
do{                                                   \
    (cpu)->memory[address]     = value & 0xFF;        \
    (cpu)->memory[address + 1] = (value >> 8) & 0xFF; \
}while(0)

#define BRANCH(cpu,conditional_logic)                                         \
do{                                                                           \
    CPU_6502_uint16_t cicles = (cpu)->instruction_table[(cpu)->opcode].cicles;\
    if(conditional_logic){                                                    \
        CPU_6502_int8_t offset = cpu->instruction_address;                    \
        CPU_6502_uint16_t address = (cpu)->program_counter + offset;          \
        if(address >> 8 == (cpu)->program_counter >> 8) cicles += 1;          \
        else                                            cicles += 2;          \
        (cpu)->program_counter = address;                                     \
    }                                                                         \
    (cpu)->instruction_cicles = cicles;                                       \
}while(0)

#define SET_FLAG(cpu,flag,conditional_logic)              \
do{                                                       \
    if(conditional_logic) (cpu)->status_register |= flag; \
    else                  (cpu)->status_register &= ~flag;\
}while(0);


typedef unsigned char  CPU_6502_uint8_t;
typedef signed char    CPU_6502_int8_t;
typedef unsigned short CPU_6502_uint16_t;
typedef signed char    CPU_6502_int16_t;
typedef unsigned int   CPU_6502_uint32_t;
typedef signed int     CPU_6502_int32_t;

typedef enum   _CPU_6502_bool CPU_6502_bool;
typedef struct _Instruction Instruction;
typedef struct _CPU_6502 CPU_6502;


typedef void (*Operation)(struct _CPU_6502 *cpu);
typedef void (*AddressMode)(struct _CPU_6502 *cpu);

enum _CPU_6502_bool{
    CPU_6502_false,
    CPU_6502_true
};

struct _Instruction{
    CPU_6502_uint16_t *name;
    Operation operation;
    AddressMode address;
    CPU_6502_uint8_t cicles;
    CPU_6502_uint8_t bytes;
};

struct _CPU_6502{

    CPU_6502_uint16_t program_counter;
    CPU_6502_uint8_t accumalator;
    CPU_6502_uint8_t x_register;
    CPU_6502_uint8_t y_register;
    CPU_6502_uint8_t status_register;
    CPU_6502_uint8_t stack_pointer;

    CPU_6502_uint8_t opcode;
    CPU_6502_uint8_t instruction_cicles;
    CPU_6502_uint16_t instruction_address;

    CPU_6502_uint32_t total_cicles;
    CPU_6502_uint32_t total_instructions;
    CPU_6502_uint16_t stack_pointer_word;
    CPU_6502_bool interrupt;

    CPU_6502_uint8_t memory[MEMORY_SIZE];
    Instruction instruction_table[TABLE_SIZE];
};

void REL(CPU_6502 *cpu);
void IMP(CPU_6502 *cpu);
void ACU(CPU_6502 *cpu);
void IME(CPU_6502 *cpu);
void ABS(CPU_6502 *cpu);
void ZP0(CPU_6502 *cpu);
void ABX(CPU_6502 *cpu);
void ABY(CPU_6502 *cpu);
void ZPX(CPU_6502 *cpu);
void ZPY(CPU_6502 *cpu);
void IND(CPU_6502 *cpu);
void IDX(CPU_6502 *cpu);
void IDY(CPU_6502 *cpu);

void CPU_6502_Init(CPU_6502 *cpu);
void CPU_6502_LoadProgram(CPU_6502 *cpu,CPU_6502_uint8_t *source,CPU_6502_uint32_t size,CPU_6502_uint16_t start_address);
void CPU_6502_Execute(CPU_6502 *cpu);
void CPU_6502_NMI(CPU_6502 *cpu);
void CPU_6502_RES(CPU_6502 *cpu);
void CPU_6502_IRQ(CPU_6502 *cpu);

#endif
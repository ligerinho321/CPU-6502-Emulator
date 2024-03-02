#include <CPU_6502.h>

//--------------------------------------------------------------------------------------------------------------------------------------
//Stack utils

static void push_16_bits(CPU_6502 *cpu,CPU_6502_uint16_t value){
    cpu->stack_pointer_word -= 2;
    cpu->memory[STACK_START + cpu->stack_pointer--] = (value >> 8) & CPU_6502_BYTE; //HB
    cpu->memory[STACK_START + cpu->stack_pointer--] = value & CPU_6502_BYTE;        //LB
}

static CPU_6502_uint16_t pop_16_bits(CPU_6502 *cpu){
    cpu->stack_pointer_word += 2;
    CPU_6502_uint16_t low_byte  = cpu->memory[STACK_START + ++cpu->stack_pointer];
    CPU_6502_uint16_t high_byte = cpu->memory[STACK_START + ++cpu->stack_pointer];
    return (high_byte << 8) | low_byte;
}

static void push_8_bits(CPU_6502 *cpu,CPU_6502_uint8_t value){
    --cpu->stack_pointer_word;
    cpu->memory[STACK_START + cpu->stack_pointer--] = value;
}

static CPU_6502_uint8_t pop_8_bits(CPU_6502 *cpu){
    ++cpu->stack_pointer_word;
    return cpu->memory[STACK_START + ++cpu->stack_pointer];
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Address Mode

//REL address relative
void REL(CPU_6502 *cpu){
    cpu->instruction_address = READ_8_BITS(cpu,cpu->program_counter + 1);
}

//IMP address implied
void IMP(CPU_6502 *cpu){
    cpu->instruction_address = 0x00;
}

//ACU address accumalator
void ACU(CPU_6502 *cpu){
    cpu->instruction_address = 0x00;
}

//IME address imediate
void IME(CPU_6502 *cpu){
    cpu->instruction_address = cpu->program_counter + 1;
}

//ABS address absolute
void ABS(CPU_6502 *cpu){ 
    cpu->instruction_address = READ_16_BITS(cpu,cpu->program_counter + 1);
}

//ZP0 address zero page
void ZP0(CPU_6502 *cpu){
    cpu->instruction_address = READ_8_BITS(cpu,cpu->program_counter + 1);
}

//ABX address absolute x
void ABX(CPU_6502 *cpu){
    cpu->instruction_address = READ_16_BITS(cpu,cpu->program_counter + 1) + cpu->x_register;
}

//ABY address absolute y
void ABY(CPU_6502 *cpu){
    cpu->instruction_address = READ_16_BITS(cpu,cpu->program_counter + 1) + cpu->y_register;
}

//ZPX address zero page x
void ZPX(CPU_6502 *cpu){
    cpu->instruction_address = READ_8_BITS(cpu,cpu->program_counter + 1) + cpu->x_register;
}

//ZPY address zero page y
void ZPY(CPU_6502 *cpu){
    cpu->instruction_address = READ_8_BITS(cpu,cpu->program_counter + 1) + cpu->y_register;
}

//IND address indirect
void IND(CPU_6502 *cpu){
    cpu->instruction_address = READ_16_BITS(cpu,READ_16_BITS(cpu,cpu->program_counter + 1));
}

//IDX address indirect x
void IDX(CPU_6502 *cpu){
    cpu->instruction_address = READ_16_BITS(cpu,READ_8_BITS(cpu,cpu->program_counter + 1) + cpu->x_register);
}

//IDY address indirect y
void IDY(CPU_6502 *cpu){
    cpu->instruction_address = READ_16_BITS(cpu,READ_8_BITS(cpu,cpu->program_counter + 1)) + cpu->y_register;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Transfer Instructions

//LDA load accumulator
void LDA(CPU_6502 *cpu){
    cpu->accumalator = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//LDX load X
void LDX(CPU_6502 *cpu){
    cpu->x_register = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//LDY load Y
void LDY(CPU_6502 *cpu){
    cpu->y_register = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->y_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->y_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//STA store accumulator
void STA(CPU_6502 *cpu){
    WRITE_8_BITS(cpu,cpu->instruction_address,cpu->accumalator);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//STX store X
void STX(CPU_6502 *cpu){
    WRITE_8_BITS(cpu,cpu->instruction_address,cpu->x_register);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//STY store Y
void STY(CPU_6502 *cpu){
    WRITE_8_BITS(cpu,cpu->instruction_address,cpu->y_register);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TAX transfer accumulator to X
void TAX(CPU_6502 *cpu){
    cpu->x_register = cpu->accumalator;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TAY transfer accumulator to Y
void TAY(CPU_6502 *cpu){
    cpu->y_register = cpu->accumalator;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->y_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->y_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TSX transfer stack pointer to X
void TSX(CPU_6502 *cpu){
    cpu->x_register = cpu->stack_pointer;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TXA transfer X to accumulator
void TXA(CPU_6502 *cpu){
    cpu->accumalator = cpu->x_register;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TXS transfer X to stack pointer
void TXS(CPU_6502 *cpu){
    push_8_bits(cpu,cpu->x_register);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//TYA transfer Y to accumulator
void TYA(CPU_6502 *cpu){
    cpu->accumalator = cpu->y_register;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Stack Instructions

//PHA push accumulator
void PHA(CPU_6502 *cpu){
    push_8_bits(cpu,cpu->accumalator);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//PHP push processor status register (with break flag set)
void PHP(CPU_6502 *cpu){
    push_8_bits(cpu,cpu->status_register | CPU_6502_FLAG_B);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//PLA pull accumulator
void PLA(CPU_6502 *cpu){
    cpu->accumalator = pop_8_bits(cpu);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//PLP pull processor status register
void PLP(CPU_6502 *cpu){
    cpu->status_register = pop_8_bits(cpu);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Decrecment and Increment

//DEC decrement (memory)
void DEC(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address) - 1;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,value == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,value & CPU_6502_BIT_7);
    WRITE_8_BITS(cpu,cpu->instruction_address,value);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//DEX decrement X
void DEX(CPU_6502 *cpu){
    cpu->x_register--;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//DEY decrement Y
void DEY(CPU_6502 *cpu){
    cpu->y_register--;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->y_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->y_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//INC increment (memory)
void INC(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address) + 1;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,value == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,value & CPU_6502_BIT_7);
    WRITE_8_BITS(cpu,cpu->instruction_address,value);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//INX increment X
void INX(CPU_6502 *cpu){
    cpu->x_register++;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//INY increment Y
void INY(CPU_6502 *cpu){
    cpu->y_register++;
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->y_register == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->y_register & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Arithmetic Operations

//ADC add with carry (prepare by CLC)
void ADC(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    CPU_6502_uint16_t result = cpu->accumalator + value + (cpu->status_register & CPU_6502_FLAG_C);
    SET_FLAG(cpu,CPU_6502_FLAG_C,result > CPU_6502_BYTE);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,(result & CPU_6502_BYTE) == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_V,~(cpu->accumalator ^ value) & (cpu->accumalator ^ result) & CPU_6502_BIT_7);
    SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
    cpu->accumalator = result & CPU_6502_BYTE;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//SBC subtract with carry (prepare by SEC)
void SBC(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    CPU_6502_uint16_t result = cpu->accumalator - value - (cpu->status_register & CPU_6502_FLAG_C);
    SET_FLAG(cpu,CPU_6502_FLAG_C,result > CPU_6502_BYTE);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,(result & CPU_6502_BYTE) == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_V,(cpu->accumalator ^ value) & (cpu->accumalator ^ result) & CPU_6502_BIT_7);
    SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
    cpu->accumalator = result & CPU_6502_BYTE;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Logical Operations

//AND and (with accumulator)
void AND(CPU_6502 *cpu){
    cpu->accumalator &= READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//EOR exclusive or (with accumulator)
void EOR(CPU_6502 *cpu){
    cpu->accumalator ^= READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//ORA (inclusive) or with accumulator
void ORA(CPU_6502 *cpu){
    cpu->accumalator |= READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;   
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Shift and Rotate Instructions

//ASL arithmetic shift left (shifts in a zero bit on the right)
void ASL(CPU_6502 *cpu){
    if(cpu->instruction_table[cpu->opcode].address == ACU){
        CPU_6502_uint8_t result = cpu->accumalator << 1;
        SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->accumalator & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        cpu->accumalator = result;
    }
    else{
        CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
        CPU_6502_uint8_t result = value << 1;
        SET_FLAG(cpu,CPU_6502_FLAG_C,value & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        WRITE_8_BITS(cpu,cpu->instruction_address,value);
    }
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//LSR logical shift right (shifts in a zero bit on the left)
void LSR(CPU_6502 *cpu){
    if(cpu->instruction_table[cpu->opcode].address == ACU){
        CPU_6502_uint8_t result = cpu->accumalator >> 1;
        SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->accumalator & CPU_6502_BIT_0);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        cpu->accumalator = result;
    }
    else{
        CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
        CPU_6502_uint8_t result = value >> 1;
        SET_FLAG(cpu,CPU_6502_FLAG_C,value & CPU_6502_BIT_0);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        WRITE_8_BITS(cpu,cpu->instruction_address,result);
    }
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//ROL rotate left (shifts in carry bit on the right)
void ROL(CPU_6502 *cpu){
    if(cpu->instruction_table[cpu->opcode].address == ACU){
        CPU_6502_uint8_t result = (cpu->accumalator << 1) | (cpu->status_register & CPU_6502_FLAG_C);
        SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->accumalator & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        cpu->accumalator = result;
    }
    else{
        CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
        CPU_6502_uint8_t result = (value << 1) | (cpu->status_register & CPU_6502_FLAG_C);
        SET_FLAG(cpu,CPU_6502_FLAG_C,value & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        WRITE_8_BITS(cpu,cpu->instruction_address,value);
    }
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//ROR rotate right (shifts in zero bit on the left)
void ROR(CPU_6502 *cpu){
    if(cpu->instruction_table[cpu->opcode].address == ACU){
        CPU_6502_uint8_t result = (cpu->accumalator >> 1) | (cpu->status_register & CPU_6502_FLAG_C);
        SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->accumalator & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        cpu->accumalator = result;
    }
    else{
        CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
        CPU_6502_uint8_t result = (value >> 1) | (cpu->status_register & CPU_6502_FLAG_C);
        SET_FLAG(cpu,CPU_6502_FLAG_C,value & CPU_6502_BIT_7);
        SET_FLAG(cpu,CPU_6502_FLAG_Z,result == CPU_6502_ZERO);
        SET_FLAG(cpu,CPU_6502_FLAG_N,result & CPU_6502_BIT_7);
        WRITE_8_BITS(cpu,cpu->instruction_address,value);
    }
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Flag Instructions

//CLC clear carry
void CLC(CPU_6502 *cpu){
    cpu->status_register &= ~CPU_6502_FLAG_C;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//CLD clear decimal (BCD arithmetics disabled)
void CLD(CPU_6502 *cpu){
    cpu->status_register &= ~CPU_6502_FLAG_D;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//CLI clear interrupt disable
void CLI(CPU_6502 *cpu){
    cpu->status_register &= ~CPU_6502_FLAG_I;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//CLV clear overflow
void CLV(CPU_6502 *cpu){
    cpu->status_register &= ~CPU_6502_FLAG_V;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//SEC set carry
void SEC(CPU_6502 *cpu){
    cpu->status_register |= CPU_6502_FLAG_C;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//SED set decimal (BCD arithmetics enabled)
void SED(CPU_6502 *cpu){
    cpu->status_register |= CPU_6502_FLAG_D;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//SEI set interrupt disable
void SEI(CPU_6502 *cpu){
    cpu->status_register |= CPU_6502_FLAG_I;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Comparations

//CMP compare with accumulator
void CMP(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->accumalator == value);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->accumalator < value);
    SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->accumalator >= value);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//CPX compare with X
void CPX(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->x_register == value);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->x_register < value);
    SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->x_register >= value);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//CPY compare with Y
void CPY(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,cpu->y_register == value);
    SET_FLAG(cpu,CPU_6502_FLAG_N,cpu->y_register < value);
    SET_FLAG(cpu,CPU_6502_FLAG_C,cpu->y_register >= value);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Conditional Branch Instructions

//BCC branch on carry clear
void BCC(CPU_6502 *cpu){BRANCH(cpu,!(cpu->status_register & CPU_6502_FLAG_C));}

//BCS branch on carry set
void BCS(CPU_6502 *cpu){BRANCH(cpu,cpu->status_register & CPU_6502_FLAG_C);}

//BEQ branch on equal (zero set)
void BEQ(CPU_6502 *cpu){BRANCH(cpu,cpu->status_register & CPU_6502_FLAG_Z);}

//BMI branch on minus (negative set)
void BMI(CPU_6502 *cpu){BRANCH(cpu,cpu->status_register & CPU_6502_FLAG_N);}

//BNE branch on not equal (zero clear)
void BNE(CPU_6502 *cpu){BRANCH(cpu,!(cpu->status_register & CPU_6502_FLAG_Z));}

//BPL branch on plus (negative clear)
void BPL(CPU_6502 *cpu){BRANCH(cpu,!(cpu->status_register & CPU_6502_FLAG_N));}

//BVC branch on overflow clear
void BVC(CPU_6502 *cpu){BRANCH(cpu,!(cpu->status_register & CPU_6502_FLAG_V));}

//BVS branch on overflow set
void BVS(CPU_6502 *cpu){BRANCH(cpu,cpu->status_register & CPU_6502_FLAG_V);}

//--------------------------------------------------------------------------------------------------------------------------------------
//Jumps and Subroutines

//JMP jump absolute
void JMP(CPU_6502 *cpu){
    cpu->program_counter = cpu->instruction_address;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//JSR jump subruntime
void JSR(CPU_6502 *cpu){
    push_16_bits(cpu,cpu->program_counter);
    cpu->program_counter = cpu->instruction_address;
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//RTS return from subruntime
void RTS(CPU_6502 *cpu){
    cpu->program_counter = pop_16_bits(cpu);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Interrupts

//BRK break / software interrupt
void BRK(CPU_6502 *cpu){
    cpu->interrupt = CPU_6502_true;
    push_16_bits(cpu,cpu->program_counter);
    push_8_bits(cpu,cpu->status_register | CPU_6502_FLAG_B);
    SET_FLAG(cpu,CPU_6502_FLAG_B | CPU_6502_FLAG_I,CPU_6502_true);
    cpu->program_counter = READ_16_BITS(cpu,IRQ_ADDRESS);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//RTI no operation
void RTI(CPU_6502 *cpu){
    cpu->status_register = pop_8_bits(cpu);
    cpu->program_counter = pop_16_bits(cpu);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Other

//BIT bit test (accumulator and memory)
void BIT(CPU_6502 *cpu){
    CPU_6502_uint8_t value = READ_8_BITS(cpu,cpu->instruction_address);
    SET_FLAG(cpu,CPU_6502_FLAG_Z,(cpu->accumalator & value) == CPU_6502_ZERO);
    SET_FLAG(cpu,CPU_6502_FLAG_V,value & CPU_6502_BIT_6);
    SET_FLAG(cpu,CPU_6502_FLAG_N,value & CPU_6502_BIT_7);
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//NOP no operation
void NOP(CPU_6502 *cpu){
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//XXX illegal
void XXX(CPU_6502 *cpu){
    cpu->instruction_cicles = cpu->instruction_table[cpu->opcode].cicles;
}

//--------------------------------------------------------------------------------------------------------------------------------------

static Instruction instruction_table[TABLE_SIZE] = {
    //                  0                           1                          2                          3                          4                          5                          6                          7                          8                          9                          A                          B                          C                          D                          E                          F
    /* 0 */ { L"BRK", BRK, IMP, 7, 1 }, { L"ORA", ORA, IDX, 6, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 3, 1 },{ L"ORA", ORA, ZP0, 3, 2 },{ L"ASL", ASL, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"PHP", PHP, IMP, 3, 1 },{ L"ORA", ORA, IME, 2, 2 },{ L"ASL", ASL, ACU, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"ORA", ORA, ABS, 4, 3 },{ L"ASL", ASL, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* 1 */ { L"BPL", BPL, REL, 2, 2 }, { L"ORA", ORA, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"ORA", ORA, ZPX, 4, 2 },{ L"ASL", ASL, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"CLC", CLC, IMP, 2, 1 },{ L"ORA", ORA, ABY, 4, 3 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"ORA", ORA, ABX, 4, 3 },{ L"ASL", ASL, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
	/* 2 */ { L"JSR", JSR, ABS, 6, 3 }, { L"AND", AND, IDX, 6, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"BIT", BIT, ZP0, 3, 2 },{ L"AND", AND, ZP0, 3, 2 },{ L"ROL", ROL, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"PLP", PLP, IMP, 4, 1 },{ L"AND", AND, IME, 2, 2 },{ L"ROL", ROL, ACU, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"BIT", BIT, ABS, 4, 3 },{ L"AND", AND, ABS, 4, 3 },{ L"ROL", ROL, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* 3 */ { L"BMI", BMI, REL, 2, 2 }, { L"AND", AND, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"AND", AND, ZPX, 4, 2 },{ L"ROL", ROL, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"SEC", SEC, IMP, 2, 1 },{ L"AND", AND, ABY, 4, 3 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"AND", AND, ABX, 4, 3 },{ L"ROL", ROL, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
	/* 4 */ { L"RTI", RTI, IMP, 6, 1 }, { L"EOR", EOR, IDX, 6, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 3, 1 },{ L"EOR", EOR, ZP0, 3, 2 },{ L"LSR", LSR, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"PHA", PHA, IMP, 3, 1 },{ L"EOR", EOR, IME, 2, 2 },{ L"LSR", LSR, ACU, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"JMP", JMP, ABS, 3, 3 },{ L"EOR", EOR, ABS, 4, 3 },{ L"LSR", LSR, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* 5 */ { L"BVC", BVC, REL, 2, 2 }, { L"EOR", EOR, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"EOR", EOR, ZPX, 4, 2 },{ L"LSR", LSR, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"CLI", CLI, IMP, 2, 1 },{ L"EOR", EOR, ABY, 4, 3 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"EOR", EOR, ABX, 4, 3 },{ L"LSR", LSR, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
	/* 6 */ { L"RTS", RTS, IMP, 6, 1 }, { L"ADC", ADC, IDX, 6, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 3, 1 },{ L"ADC", ADC, ZP0, 3, 2 },{ L"ROR", ROR, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"PLA", PLA, IMP, 4, 1 },{ L"ADC", ADC, IME, 2, 2 },{ L"ROR", ROR, ACU, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"JMP", JMP, IND, 5, 3 },{ L"ADC", ADC, ABS, 4, 3 },{ L"ROR", ROR, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* 7 */ { L"BVS", BVS, REL, 2, 2 }, { L"ADC", ADC, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"ADC", ADC, ZPX, 4, 2 },{ L"ROR", ROR, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"SEI", SEI, IMP, 2, 1 },{ L"ADC", ADC, ABY, 4, 3 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"ADC", ADC, ABX, 4, 3 },{ L"ROR", ROR, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
	/* 8 */ { L"???", NOP, IME, 2, 2 }, { L"STA", STA, IDX, 6, 2 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 6, 1 },{ L"STY", STY, ZP0, 3, 2 },{ L"STA", STA, ZP0, 3, 2 },{ L"STX", STX, ZP0, 3, 2 },{ L"???", XXX, IMP, 3, 1 },{ L"DEY", DEY, IMP, 2, 1 },{ L"???", NOP, IMP, 2, 1 },{ L"TXA", TXA, IMP, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"STY", STY, ABS, 4, 3 },{ L"STA", STA, ABS, 4, 3 },{ L"STX", STX, ABS, 4, 3 },{ L"???", XXX, IMP, 4, 1 },
	/* 9 */ { L"BCC", BCC, REL, 2, 2 }, { L"STA", STA, IDY, 6, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 6, 1 },{ L"STY", STY, ZPX, 4, 2 },{ L"STA", STA, ZPX, 4, 2 },{ L"STX", STX, ZPY, 4, 2 },{ L"???", XXX, IMP, 4, 1 },{ L"TYA", TYA, IMP, 2, 1 },{ L"STA", STA, ABY, 5, 3 },{ L"TXS", TXS, IMP, 2, 1 },{ L"???", XXX, IMP, 5, 1 },{ L"???", NOP, IMP, 5, 1 },{ L"STA", STA, ABX, 5, 3 },{ L"???", XXX, IMP, 5, 1 },{ L"???", XXX, IMP, 5, 1 },
	/* A */ { L"LDY", LDY, IME, 2, 2 }, { L"LDA", LDA, IDX, 6, 2 },{ L"LDX", LDX, IME, 2, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"LDY", LDY, ZP0, 3, 2 },{ L"LDA", LDA, ZP0, 3, 2 },{ L"LDX", LDX, ZP0, 3, 2 },{ L"???", XXX, IMP, 3, 1 },{ L"TAY", TAY, IMP, 2, 1 },{ L"LDA", LDA, IME, 2, 2 },{ L"TAX", TAX, IMP, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"LDY", LDY, ABS, 4, 3 },{ L"LDA", LDA, ABS, 4, 3 },{ L"LDX", LDX, ABS, 4, 3 },{ L"???", XXX, IMP, 4, 1 },
	/* B */ { L"BCS", BCS, REL, 2, 2 }, { L"LDA", LDA, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 5, 1 },{ L"LDY", LDY, ZPX, 4, 2 },{ L"LDA", LDA, ZPX, 4, 2 },{ L"LDX", LDX, ZPY, 4, 2 },{ L"???", XXX, IMP, 4, 1 },{ L"CLV", CLV, IMP, 2, 1 },{ L"LDA", LDA, ABY, 4, 3 },{ L"TSX", TSX, IMP, 2, 1 },{ L"???", XXX, IMP, 4, 1 },{ L"LDY", LDY, ABX, 4, 3 },{ L"LDA", LDA, ABX, 4, 3 },{ L"LDX", LDX, ABY, 4, 3 },{ L"???", XXX, IMP, 4, 1 },
	/* C */ { L"CPY", CPY, IME, 2, 2 }, { L"CMP", CMP, IDX, 6, 2 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"CPY", CPY, ZP0, 3, 2 },{ L"CMP", CMP, ZP0, 3, 2 },{ L"DEC", DEC, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"INY", INY, IMP, 2, 1 },{ L"CMP", CMP, IME, 2, 2 },{ L"DEX", DEX, IMP, 2, 1 },{ L"???", XXX, IMP, 2, 1 },{ L"CPY", CPY, ABS, 4, 3 },{ L"CMP", CMP, ABS, 4, 3 },{ L"DEC", DEC, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* D */ { L"BNE", BNE, REL, 2, 2 }, { L"CMP", CMP, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"CMP", CMP, ZPX, 4, 2 },{ L"DEC", DEC, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"CLD", CLD, IMP, 2, 1 },{ L"CMP", CMP, ABY, 4, 3 },{ L"NOP", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"CMP", CMP, ABX, 4, 3 },{ L"DEC", DEC, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
	/* E */ { L"CPX", CPX, IME, 2, 2 }, { L"SBC", SBC, IDX, 6, 2 },{ L"???", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"CPX", CPX, ZP0, 3, 2 },{ L"SBC", SBC, ZP0, 3, 2 },{ L"INC", INC, ZP0, 5, 2 },{ L"???", XXX, IMP, 5, 1 },{ L"INX", INX, IMP, 2, 1 },{ L"SBC", SBC, IME, 2, 2 },{ L"NOP", NOP, IMP, 2, 1 },{ L"???", SBC, IMP, 2, 1 },{ L"CPX", CPX, ABS, 4, 3 },{ L"SBC", SBC, ABS, 4, 3 },{ L"INC", INC, ABS, 6, 3 },{ L"???", XXX, IMP, 6, 1 },
	/* F */ { L"BEQ", BEQ, REL, 2, 2 }, { L"SBC", SBC, IDY, 5, 2 },{ L"???", XXX, IMP, 2, 1 },{ L"???", XXX, IMP, 8, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"SBC", SBC, ZPX, 4, 2 },{ L"INC", INC, ZPX, 6, 2 },{ L"???", XXX, IMP, 6, 1 },{ L"SED", SED, IMP, 2, 1 },{ L"SBC", SBC, ABY, 4, 3 },{ L"NOP", NOP, IMP, 2, 1 },{ L"???", XXX, IMP, 7, 1 },{ L"???", NOP, IMP, 4, 1 },{ L"SBC", SBC, ABX, 4, 3 },{ L"INC", INC, ABX, 7, 3 },{ L"???", XXX, IMP, 7, 1 },
};

void CPU_6502_Init(CPU_6502 *cpu){
    cpu->program_counter     = 0x00;
    cpu->accumalator         = 0x00;
    cpu->x_register          = 0x00;
    cpu->y_register          = 0x00;
    cpu->status_register     = 0x00;
    cpu->stack_pointer       = 0xFF;
    
    cpu->opcode              = 0x00;
    cpu->instruction_cicles  = 0x00;
    cpu->instruction_address = 0x00;

    cpu->total_cicles        = 0x00;
    cpu->total_instructions  = 0x00;
    cpu->stack_pointer_word = STACK_END;
    cpu->interrupt = CPU_6502_false;

    memset(cpu->memory,0,sizeof(CPU_6502_uint8_t) * MEMORY_SIZE);
    memcpy(cpu->instruction_table,instruction_table,sizeof(Instruction) * TABLE_SIZE);
}

void CPU_6502_LoadProgram(CPU_6502 *cpu,CPU_6502_uint8_t *source,CPU_6502_uint32_t size,CPU_6502_uint16_t start_address){
    if(start_address + size > MEMORY_SIZE){
        size = MEMORY_SIZE - start_address;
    }
    memcpy(cpu->memory + start_address,source,sizeof(CPU_6502_uint8_t) * size);
}

void CPU_6502_Execute(CPU_6502 *cpu){
    cpu->interrupt = CPU_6502_false;

    cpu->opcode = READ_8_BITS(cpu,cpu->program_counter);

    cpu->instruction_table[cpu->opcode].address(cpu);

    cpu->program_counter += cpu->instruction_table[cpu->opcode].bytes;

    cpu->instruction_table[cpu->opcode].operation(cpu);

    cpu->total_cicles += cpu->instruction_cicles;
    cpu->total_instructions++;
}


void CPU_6502_RES(CPU_6502 *cpu){
    cpu->program_counter = READ_16_BITS(cpu,RES_ADDRESS);
    cpu->stack_pointer = 0xFF;
    cpu->status_register = 0x00;
    cpu->y_register = 0x00;
    cpu->x_register = 0x00;
    cpu->accumalator = 0x00;

    cpu->total_cicles = 0x00;
    cpu->total_instructions = 0x00;
    cpu->stack_pointer_word = STACK_END;
    cpu->interrupt = CPU_6502_true;
}

void CPU_6502_NMI(CPU_6502 *cpu){
    cpu->interrupt = CPU_6502_true;
    push_16_bits(cpu,cpu->program_counter);
    SET_FLAG(cpu,CPU_6502_FLAG_B,CPU_6502_false);
    push_8_bits(cpu,cpu->status_register);
    SET_FLAG(cpu,CPU_6502_FLAG_I,CPU_6502_true);
    cpu->program_counter = READ_16_BITS(cpu,NMI_ADDRESS);
    cpu->instruction_cicles = 7;
}

void CPU_6502_IRQ(CPU_6502 *cpu){
    if(cpu->status_register & CPU_6502_FLAG_I) return;
    cpu->interrupt = CPU_6502_true;
    push_16_bits(cpu,cpu->program_counter);
    SET_FLAG(cpu,CPU_6502_FLAG_B,CPU_6502_false);
    push_8_bits(cpu,cpu->status_register);
    SET_FLAG(cpu,CPU_6502_FLAG_I,CPU_6502_true);
    cpu->program_counter = READ_16_BITS(cpu,IRQ_ADDRESS);
    cpu->instruction_cicles = 7;
}

/***************************************************

	File: Execution.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include "Execution.h"
#include "Instruction.h"
#include "CMP.h"

extern int Address[1024];
extern map< int,char > Memory;
extern int reg[32], PC;
extern bool Halt;

bool WriteToZeroDetect(int WriteDes){
	if(WriteDes==0){
		PC += 4;
		return true;
	}
	return false;
}

void Execute(Instruction ins){
	int Word, K = 0;
	switch(ins.opcode){
		case 0x00:
			if(ins.funct!=8 && WriteToZeroDetect(ins.rd)) return;
			PC += 4;
			switch(ins.funct){
				case 0x20: // add
					reg[ins.rd] = reg[ins.rs] + reg[ins.rt];
				break;
				case 0x21: // addu
					reg[ins.rd] = reg[ins.rs] + reg[ins.rt];
					break;
				case 0x22: // sub
					reg[ins.rd] = reg[ins.rs] - reg[ins.rt];
					break;
				case 0x24: // and
					reg[ins.rd] = reg[ins.rs] & reg[ins.rt];					
					break;
				case 0x25: // or
					reg[ins.rd] = reg[ins.rs] | reg[ins.rt];
					break;
				case 0x26: // xor
					reg[ins.rd] = reg[ins.rs] ^ reg[ins.rt];
					break;
				case 0x27: // nor
					reg[ins.rd] = ~(reg[ins.rs] | reg[ins.rt]);
					break;
				case 0x28: // nand
					reg[ins.rd] = ~(reg[ins.rs] & reg[ins.rt]);
					break;
				case 0x2A: // slt
					reg[ins.rd] = (reg[ins.rs] < reg[ins.rt]);
					break;
				case 0x00: // sll
					reg[ins.rd] = reg[ins.rt] << ins.shamt;
					break;
				case 0x02: // srl
					reg[ins.rd] = (unsigned int)reg[ins.rt] >> ins.shamt;
					break;
				case 0x03: // sra
					reg[ins.rd] = reg[ins.rt] >> ins.shamt;
					break;
				case 0x08: // jr
					PC = reg[ins.rs];
					break;
			}
			break;
		case 0x08: // addi
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = reg[ins.rs] + (int) ins.C;
			PC += 4;
			break;
		case 0x09: // addiu
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = reg[ins.rs] + (int) ins.C;
			PC += 4;
			break;
		case 0x23: // lw
			Word = 0;
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			if(WriteToZeroDetect(ins.rt)) return;
			for(int i=0; i<4; i++)
				Word = (Word << 8) | (unsigned char)Memory[K+i];
			reg[ins.rt] = Word;
			PC += 4;
			break;
		case 0x21: // lh
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			Word = Memory[K];
			if(WriteToZeroDetect(ins.rt)) return;
			for(int i=1; i<2; i++)
				Word = (Word << 8) | (unsigned char)Memory[K+i];
			reg[ins.rt] = Word;
			PC += 4;
			break;
		case 0x25: // lhu
			Word = 0;
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			if(WriteToZeroDetect(ins.rt)) return;
			for(int i=0; i<2; i++)
				Word = (Word << 8) | (unsigned char)Memory[K+i];
			reg[ins.rt] = Word;
			PC += 4;
			break;
		case 0x20: // lb
			Word = 0;
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = Memory[K];
			PC += 4;
			break;
		case 0x24: // lbu
			Word = 0;
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = (unsigned char)Memory[K];
			PC += 4;
			break;
		case 0x2B: // sw
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			for(int i=0; i<4; i++)
				Memory[K+i] = (char)(reg[ins.rt] >> (8*(3-i)));
			PC += 4;
			break;
		case 0x29: // sh
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			for(int i=0; i<2; i++)
				Memory[K+i] = (char)(reg[ins.rt] >> (8*(1-i)));
			PC += 4;
			break;
		case 0x28: // sb
			K = reg[ins.rs] + (int) ins.C;
			Calculate_CMP(K, false);
			Memory[K] = (char)reg[ins.rt];
			PC += 4;
			break;
		case 0x0F: // lui
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = (int)ins.C << 16;
			PC += 4;
			break;
		case 0x0C: // andi
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			reg[ins.rt] = reg[ins.rs] & K;
			PC += 4;
			break;
		case 0x0D: // ori
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			reg[ins.rt] = reg[ins.rs] | K;
			PC += 4;
			break;
		case 0x0E: // nori
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			reg[ins.rt] = ~(reg[ins.rs] | K);
			PC += 4;
			break;
		case 0x0A: // slti
			if(WriteToZeroDetect(ins.rt)) return;
			reg[ins.rt] = (reg[ins.rs] < ins.C);
			PC += 4;
			break;
		case 0x04: // beq
			if(reg[ins.rs] == reg[ins.rt]){
				PC = PC + 4 + (int)ins.C*4;
			}
			else{
				PC += 4;
			}
			break;
		case 0x05: // bne
			if(reg[ins.rs] != reg[ins.rt]){
				PC = PC + 4 + (int)ins.C*4;
			}
			else{
				PC += 4;
			}
			break;
		case 0x07: // bgtz
			if(reg[ins.rs] > 0){
				PC = PC + 4 + (int)ins.C*4;
			}
			else{
				PC += 4;
			}
			break;
		case 0x02: // j
			ins.C = (unsigned)(ins.Word << 6) >> 6;
			K = ((PC+4) >> 28) << 28;
			PC =  K + (ins.C << 2);
			break;
		case 0x03: // jal
			reg[31] = PC + 4;
			ins.C = (unsigned)(ins.Word << 6) >> 6;
			K = ((PC+4) >> 28) << 28;
			PC =  K + (ins.C << 2);
			break;
		case 0x3F: // halt
			Halt = true;
			break;
	}
}
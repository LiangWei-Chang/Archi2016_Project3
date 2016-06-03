/***************************************************

	File: Execution.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include "Execution.h"
#include "GlobalVar.h"

bool WriteToZeroDetect(int WriteDes){
	if(WriteDes==0){
		Global::PC += 4;
		return true;
	}
	return false;
}

void Execute(Instruction ins){
	int Word, K;
	switch(ins.opcode){
		case 0x00:
			if(ins.funct!=8 && WriteToZeroDetect(ins.rd)) return;
			Global::PC += 4;
			switch(ins.funct){
				case 0x20: // add
					Global::reg[ins.rd] = Global::reg[ins.rs] + Global::reg[ins.rt];
				break;
				case 0x21: // addu
					Global::reg[ins.rd] = Global::reg[ins.rs] + Global::reg[ins.rt];
					break;
				case 0x22: // sub
					Global::reg[ins.rd] = Global::reg[ins.rs] - Global::reg[ins.rt];
					break;
				case 0x24: // and
					Global::reg[ins.rd] = Global::reg[ins.rs] & Global::reg[ins.rt];					
					break;
				case 0x25: // or
					Global::reg[ins.rd] = Global::reg[ins.rs] | Global::reg[ins.rt];
					break;
				case 0x26: // xor
					Global::reg[ins.rd] = Global::reg[ins.rs] ^ Global::reg[ins.rt];
					break;
				case 0x27: // nor
					Global::reg[ins.rd] = ~(Global::reg[ins.rs] | Global::reg[ins.rt]);
					break;
				case 0x28: // nand
					Global::reg[ins.rd] = ~(Global::reg[ins.rs] & Global::reg[ins.rt]);
					break;
				case 0x2A: // slt
					Global::reg[ins.rd] = (Global::reg[ins.rs] < Global::reg[ins.rt]);
					break;
				case 0x00: // sll
					Global::reg[ins.rd] = Global::reg[ins.rt] << ins.shamt;
					break;
				case 0x02: // srl
					Global::reg[ins.rd] = (unsigned int)Global::reg[ins.rt] >> ins.shamt;
					break;
				case 0x03: // sra
					Global::reg[ins.rd] = Global::reg[ins.rt] >> ins.shamt;
					break;
				case 0x08: // jr
					Global::PC = Global::reg[ins.rs];
					break;
			}
			break;
		case 0x08: // addi
			if(WriteToZeroDetect(ins.rt)) return;
			Global::reg[ins.rt] = Global::reg[ins.rs] + (int) ins.C;
			Global::PC += 4;
			break;
		case 0x09: // addiu
			if(WriteToZeroDetect(ins.rt)) return;
			Global::reg[ins.rt] = Global::reg[ins.rs] + (int) ins.C;
			Global::PC += 4;
			break;
		case 0x23: // lw
			if(WriteToZeroDetect(ins.rt)) return;
			Word = 0;
			K = Global::reg[ins.rs] + (int) ins.C;
			for(int i=0; i<4; i++)
				Word = (Word << 8) | (unsigned char)Global::Memory[K+i];
			Global::reg[ins.rt] = Word;
			Global::PC += 4;
			break;
		case 0x21: // lh
			if(WriteToZeroDetect(ins.rt)) return;
			K = Global::reg[ins.rs] + (int) ins.C;
			Word = Global::Memory[K];
			for(int i=1; i<2; i++)
				Word = (Word << 8) | (unsigned char)Global::Memory[K+i];
			Global::reg[ins.rt] = Word;
			Global::PC += 4;
			break;
		case 0x25: // lhu
			if(WriteToZeroDetect(ins.rt)) return;
			Word = 0;
			K = Global::reg[ins.rs] + (int) ins.C;
			for(int i=0; i<2; i++)
				Word = (Word << 8) | (unsigned char)Global::Memory[K+i];
			Global::reg[ins.rt] = Word;
			Global::PC += 4;
			break;
		case 0x20: // lb
			if(WriteToZeroDetect(ins.rt)) return;
			Word = 0;
			K = Global::reg[ins.rs] + (int) ins.C;
			Global::reg[ins.rt] = Global::Memory[K];
			Global::PC += 4;
			break;
		case 0x24: // lbu
			if(WriteToZeroDetect(ins.rt)) return;
			Word = 0;
			K = Global::reg[ins.rs] + (int) ins.C;
			Global::reg[ins.rt] = (unsigned char)Global::Memory[K];
			Global::PC += 4;
			break;
		case 0x2B: // sw
			K = Global::reg[ins.rs] + (int) ins.C;
			for(int i=0; i<4; i++)
				Global::Memory[K+i] = (char)(Global::reg[ins.rt] >> (8*(3-i)));
			Global::PC += 4;
			break;
		case 0x29: // sh
			K = Global::reg[ins.rs] + (int) ins.C;
			for(int i=0; i<2; i++)
				Global::Memory[K+i] = (char)(Global::reg[ins.rt] >> (8*(1-i)));
			Global::PC += 4;
			break;
		case 0x28: // sb
			K = Global::reg[ins.rs] + (int) ins.C;
			Global::Memory[K] = (char)Global::reg[ins.rt];
			Global::PC += 4;
			break;
		case 0x0F: // lui
			if(WriteToZeroDetect(ins.rt)) return;
			Global::reg[ins.rt] = (int)ins.C << 16;
			Global::PC += 4;
			break;
		case 0x0C: // andi
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			Global::reg[ins.rt] = Global::reg[ins.rs] & K;
			Global::PC += 4;
			break;
		case 0x0D: // ori
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			Global::reg[ins.rt] = Global::reg[ins.rs] | K;
			Global::PC += 4;
			break;
		case 0x0E: // nori
			if(WriteToZeroDetect(ins.rt)) return;
			K = ((unsigned int)ins.C << 16) >> 16;
			Global::reg[ins.rt] = ~(Global::reg[ins.rs] | K);
			Global::PC += 4;
			break;
		case 0x0A: // slti
			if(WriteToZeroDetect(ins.rt)) return;
			Global::reg[ins.rt] = (Global::reg[ins.rs] < ins.C);
			Global::PC += 4;
			break;
		case 0x04: // beq
			if(Global::reg[ins.rs] == Global::reg[ins.rt]){
				Global::PC = Global::PC + 4 + (int)ins.C*4;
			}
			else{
				Global::PC += 4;
			}
			break;
		case 0x05: // bne
			if(Global::reg[ins.rs] != Global::reg[ins.rt]){
				Global::PC = Global::PC + 4 + (int)ins.C*4;
			}
			else{
				Global::PC += 4;
			}
			break;
		case 0x07: // bgtz
			if(Global::reg[ins.rs] > 0){
				Global::PC = Global::PC + 4 + (int)ins.C*4;
			}
			else{
				Global::PC += 4;
			}
			break;
		case 0x02: // j
			ins.C = (unsigned)(ins.Word << 6) >> 6;
			K = ((Global::PC+4) >> 28) << 28;
			Global::PC =  K + (ins.C << 2);
			break;
		case 0x03: // jal
			Global::reg[31] = Global::PC + 4;
			ins.C = (unsigned)(ins.Word << 6) >> 6;
			K = ((Global::PC+4) >> 28) << 28;
			Global::PC =  K + (ins.C << 2);
			break;
		case 0x3F: // halt
			Global::Halt = true;
			break;
	}
}
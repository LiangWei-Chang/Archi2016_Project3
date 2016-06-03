/***************************************************

	File: Binary2Assembly.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include "Binary2Assembly.h"
#include "GlobalVar.h"
#include "Execution.h"

void Binary2Assembly(){
	Instruction ins;
	ins.Word = Global::Address[Global::PC];
	// Calculate opcode
	ins.opcode = ((unsigned int) ins.Word) >> 26;
	// Calculate rs
	ins.rs = ((unsigned int) (ins.Word << 6)) >> 27;
	// Calculate rt
	ins.rt = ((unsigned int) (ins.Word << 11)) >> 27;
	// Calculate rd
	ins.rd = ((unsigned int) (ins.Word << 16)) >> 27;
	// Calculate shamt
	ins.shamt = ((unsigned int) (ins.Word << 21)) >> 27;
	// Calculate funct
	ins.funct = ((unsigned int) (ins.Word << 26)) >> 26;
	// Calculate C
	ins.C = (ins.Word << 16) >> 16;

	Execute(ins);
}
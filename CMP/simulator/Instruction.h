/***************************************************

	File: Instruction.h

	Author: PinYo

***************************************************/

#ifndef Instruction_h
#define Instruction_h

#include <map>
#include <vector>

using namespace std;

class Instruction{
public:
	int Word, opcode, rs, rt, rd, shamt, funct;
	short C;
	Instruction(){
		Word = 0;
		opcode = 0;
		rs = 0;
		rt = 0;
		shamt = 0;
		funct = 0;
		C = 0;
	}
};

#endif
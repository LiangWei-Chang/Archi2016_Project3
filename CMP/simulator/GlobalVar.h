/***************************************************

	File: GlobalVar.h

	Author: PinYo

***************************************************/

#ifndef GlobalVar_h
#define GlobalVar_h

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

class Global{
public:
	static int Address[1024];
	static map< int,char > Memory;
	static int reg[32], PC;
	static bool Halt;
};

#endif
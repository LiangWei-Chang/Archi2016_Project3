/***************************************************

	File: GlobalVar.cpp

	Author: PinYo

***************************************************/

#include "GlobalVar.h"


int Global::Address[1024];
map< int,char > Global::Memory;
int Global::reg[32], Global::PC;
bool Global::Halt;
/***************************************************

	File: CMP.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include "CMP.h"
#include "Instruction.h"


extern int I_Memory_size, D_Memory_size, I_Memory_pagesize, D_Memory_pagesize, I_cache_size, I_cache_blocksize, I_cache_nway, D_cache_size, D_cache_blocksize, D_cache_nway;

void Calculate_CMP(){
	std::cout << I_Memory_size << " " << D_cache_size << endl;
}
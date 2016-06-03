/***************************************************

	File: CMP.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include <vector>
#include "CMP.h"
#include "Instruction.h"

using namespace std;

extern int I_Memory_size, D_Memory_size, I_Memory_pagesize, D_Memory_pagesize, I_cache_size, I_cache_blocksize, I_cache_nway, D_cache_size, D_cache_blocksize, D_cache_nway;
extern int I_cache_hits, I_cache_misses, D_cache_hits, D_cache_misses, I_TLB_hits, I_TLB_misses, D_TLB_hits, D_TLB_misses, I_PT_hits, I_PT_misses, D_PT_hits, D_PT_misses;
vector<TLB_entry> ITLB, DTLB;
vector<PT_entry> IPageTable, DPageTable;
vector<vector<Cache_entry>> ICache, DCache;
vector<Memory_entry> IMemory, DMemory;

void InitializeCMP(){
	IPageTable.resize(1024/I_Memory_pagesize);
	DPageTable.resize(1024/D_Memory_pagesize);
	ITLB.resize(IPageTable.size()/4);
	DTLB.resize(DPageTable.size()/4);
	ICache.resize(I_cache_nway);
	for(int i=0; i<(int)ICache.size(); i++){
		ICache[i].resize(I_cache_size/I_cache_nway/I_cache_blocksize);
	}
	DCache.resize(D_cache_nway);
	for(int i=0; i<(int)DCache.size(); i++){
		DCache[i].resize(D_cache_size/D_cache_nway/D_cache_blocksize);
	}
	IMemory.resize(I_Memory_size/I_Memory_pagesize);
	DMemory.resize(D_Memory_size/D_Memory_pagesize);
}

void Calculate_CMP(){
	InitializeCMP();
	
}
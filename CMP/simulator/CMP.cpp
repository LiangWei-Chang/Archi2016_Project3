/***************************************************

	File: CMP.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include <vector>
#include <cmath>
#include "CMP.h"
#include "Instruction.h"

using namespace std;

extern int I_Memory_size, D_Memory_size, I_Memory_pagesize, D_Memory_pagesize, I_cache_size, I_cache_blocksize, I_cache_nway, D_cache_size, D_cache_blocksize, D_cache_nway;
extern int I_cache_hits, I_cache_misses, D_cache_hits, D_cache_misses, I_TLB_hits, I_TLB_misses, D_TLB_hits, D_TLB_misses, I_PT_hits, I_PT_misses, D_PT_hits, D_PT_misses;
extern int Cycle;

extern vector<TLB_entry> ITLB, DTLB;
extern vector<PT_entry> IPageTable, DPageTable;
extern vector<vector<Cache_entry>> ICache, DCache;
extern vector<Memory_entry> IMemory, DMemory;

int VPN, PPN = 0, PA = 0;

bool ITLBIsFull(){
	for(int i=0; i < (int)ITLB.size(); i++){
		if(!ITLB[i].valid){
			return false;
		}
	}
	return true;
}

bool DTLBIsFull(){
	for(int i=0; i < (int)DTLB.size(); i++){
		if(!DTLB[i].valid){
			return false;
		}
	}
	return true;
}

bool IMemoryIsFull(){
	for(int i=0; i < (int)IMemory.size(); i++){
		if(!IMemory[i].valid){
			return false;
		}
	}
	return true;
}

bool DMemoryIsFull(){
	for(int i=0; i < (int)DMemory.size(); i++){
		if(!DMemory[i].valid){
			return false;
		}
	}
	return true;
}

void UpdateITLB(){
	if(!ITLBIsFull()){ // ITLB Not Full
		for(int i=0; i< (int)ITLB.size(); i++){
			if(!ITLB[i].valid){
				ITLB[i].valid = true;
				ITLB[i].VPN = VPN;
				ITLB[i].PPN = PPN;
				ITLB[i].last_cycle_used = Cycle;
				break;
			}
		}
	}
	else { // ITLB Is Full
		int Min = 1000000, IndexToBeChanged = 0;
		for(int i=0; i< (int)ITLB.size(); i++){
			if(ITLB[i].last_cycle_used < Min){
				Min = ITLB[i].last_cycle_used;
				IndexToBeChanged = i;
			}
		}
		ITLB[IndexToBeChanged].valid = true;
		ITLB[IndexToBeChanged].VPN = VPN;
		ITLB[IndexToBeChanged].PPN = PPN;
		ITLB[IndexToBeChanged].last_cycle_used = Cycle;
	}
}

void UpdateDTLB(){
	if(!DTLBIsFull()){ // DTLB Not Full
		for(int i=0; i< (int)DTLB.size(); i++){
			if(!DTLB[i].valid){
				DTLB[i].valid = true;
				DTLB[i].VPN = VPN;
				DTLB[i].PPN = PPN;
				DTLB[i].last_cycle_used = Cycle;
				break;
			}
		}
	}
	else { // DTLB Is Full
		int Min = 1000000, IndexToBeChanged = 0;
		for(int i=0; i< (int)DTLB.size(); i++){
			if(DTLB[i].last_cycle_used < Min){
				Min = DTLB[i].last_cycle_used;
				IndexToBeChanged = i;
			}
		}
		DTLB[IndexToBeChanged].valid = true;
		DTLB[IndexToBeChanged].VPN = VPN;
		DTLB[IndexToBeChanged].PPN = PPN;
		DTLB[IndexToBeChanged].last_cycle_used = Cycle;
	}
}

void UpdateIPT(){
	IPageTable[VPN].valid = true;
	IPageTable[VPN].PPN = PPN;
}

void UpdateDPT(){
	DPageTable[VPN].valid = true;
	DPageTable[VPN].PPN = PPN;
}

void UpdateICache(int index, int tag){
	int flag = 0;
	bool full = true, allValid = true;
	for(int i=0; i < (int)ICache[index].size(); i++){
		if(!ICache[index][i].valid){
			allValid = false;
			ICache[index][i].valid = true;
			ICache[index][i].tag = tag;
			ICache[index][i].MRU = true;
			flag = i;
			break;
		}
	}
	if(allValid){
		for(int i=0; i < (int)ICache[index].size(); i++){
			if(!ICache[index][i].MRU){
				ICache[index][i].MRU = true;
				ICache[index][i].valid = true;
				ICache[index][i].tag = tag;
				flag = i;
				break;
			}
		}
	}
	for(int i=0; i < (int)ICache[index].size(); i++){
		if(!ICache[index][i].MRU){
			full = false;
		}
	}
	if(full){ // All full
		for(int i=0; i < (int)ICache[index].size(); i++){
			if(i != flag || ICache[index].size() == 1){
				ICache[index][i].MRU = false;
			}
		}
	}
}

void UpdateDCache(int index, int tag){
	int flag = 0;
	bool full = true, allValid = true;
	for(int i=0; i < (int)DCache[index].size(); i++){
		if(!DCache[index][i].valid){
			allValid = false;
			DCache[index][i].valid = true;
			DCache[index][i].tag = tag;
			DCache[index][i].MRU = true;
			flag = i;
			break;
		}
	}
	if(allValid){
		for(int i=0; i < (int)DCache[index].size(); i++){
			if(!DCache[index][i].MRU){
				DCache[index][i].MRU = true;
				DCache[index][i].valid = true;
				DCache[index][i].tag = tag;
				flag = i;
				break;
			}
		}
	}
	for(int i=0; i < (int)DCache[index].size(); i++){
		if(!DCache[index][i].MRU){
			full = false;
		}
	}
	if(full){ // All full
		for(int i=0; i < (int)DCache[index].size(); i++){
			if(i != flag || DCache[index].size() == 1){
				DCache[index][i].MRU = false;
			}
		}
	}
}

void I_Hit_Misses(int VA){
	int BlockOffsetBits = (int) log2((double)ICache[0].size());
	int IndexBits = (int) log2((double)ICache.size());
	int TagBits = 32 - IndexBits - BlockOffsetBits;
	bool TLB_Hit = false, PT_Hit = false, Cache_Hit = false;

	VPN = VA / I_Memory_pagesize;
	// Search TLB
	for(int i = 0; i < (int)ITLB.size(); i++){
		if(ITLB[i].valid){
			if(ITLB[i].VPN == VPN){ // TLB Hit
				TLB_Hit = true;
				I_TLB_hits++;
				PPN = ITLB[i].PPN;
				ITLB[i].last_cycle_used = Cycle;
				break;
			}
		}
	}

	// TLB Miss
	if(!TLB_Hit){
		I_TLB_misses++;
		if(IPageTable[VPN].valid){ // PT Hit
			PT_Hit = true;
			I_PT_hits++;
			PPN = IPageTable[VPN].PPN;
			IMemory[PPN].last_cycle_used = Cycle;
		}
		else { // PT Miss
			I_PT_misses++;
			// Update IMemory
			if(!IMemoryIsFull()){ // IMemory Not Full
				for(int i=0; i < (int)IMemory.size(); i++){
					if(!IMemory[i].valid){
						IMemory[i].valid = true;
						IMemory[i].last_cycle_used = Cycle;
						PPN = i;
						break;
					}
				}
				// Update ITLB
				UpdateITLB();
				// Update IPT
				UpdateIPT();
			}
			else { // IMemory Is Full
				int Min = 1000000, Index = 0;
				for(int i=0; i < (int)IMemory.size(); i++){
					if(IMemory[i].last_cycle_used < Min){
						Min = IMemory[i].last_cycle_used;
						Index = i;
					}
				}
				IMemory[Index].valid = true;
				IMemory[Index].last_cycle_used = Cycle;
				PPN = Index;
				// Clear ITLB
				for(int i=0; i < (int)ITLB.size(); i++){
					if(ITLB[i].valid && ITLB[i].PPN == PPN){
						ITLB[i].valid = false;
					}
				}
				// Clear IPT
				for(int i=0; i < (int)IPageTable.size(); i++){
					if(IPageTable[i].valid && IPageTable[i].PPN == PPN){
						IPageTable[i].valid = false;
					}
				}
				// Clear ICache
				for(int i=0; i < (int)ICache.size(); i++){
					for(int j=0; j < (int)ICache[i].size(); j++){
						if(ICache[i][j].valid){
							int tempPA = 0, tempPPN = 0;
							tempPA = tempPA | (ICache[i][j].tag << (32 - TagBits));
							tempPA = tempPA | (i << (32 - TagBits - IndexBits));
							tempPA = tempPA | j;
							tempPPN = tempPA  / I_Memory_pagesize;
							if(tempPPN == PPN){
								ICache[i][j].valid = false;
								ICache[i][j].MRU = false;
							}
						}
					}
				}
				// Update ITLB
				UpdateITLB();
				// Update IPT
				UpdateIPT();
			}
		}
	}
	// Calculate PA
	PA = PPN * I_Memory_pagesize + (VA % I_Memory_pagesize); 

	// Search Cache
	int tag = (PA/I_cache_blocksize) / (int)ICache.size();
	int index = (PA/I_cache_blocksize) % (int)ICache.size();
	for(int i=0; i < (int)ICache[index].size(); i++){
		if(ICache[index][i].valid){
			if(ICache[index][i].tag == tag){ // ICache Hit
				I_cache_hits++;
				if(ICache[index].size()!=1){
					ICache[index][i].MRU = true;
				}
				Cache_Hit = true;
				break;
			}
		}
	}
	// ICache Miss
	if(!Cache_Hit){
		I_cache_misses++;
		UpdateICache(index, tag);
	}
}

void D_Hit_Misses(int VA){
	int BlockOffsetBits = (int) log2((double)DCache[0].size());
	int IndexBits = (int) log2((double)DCache.size());
	int TagBits = 32 - IndexBits - BlockOffsetBits;
	bool TLB_Hit = false, PT_Hit = false, Cache_Hit = false;
	VPN = VA / D_Memory_pagesize;
	
	// Search TLB
	for(int i = 0; i < (int)DTLB.size(); i++){
		if(DTLB[i].valid){
			if(DTLB[i].VPN == VPN){ // TLB Hit
				TLB_Hit = true;
				D_TLB_hits++;
				PPN = DTLB[i].PPN;
				DTLB[i].last_cycle_used = Cycle;
				break;
			}
		}
	}

	// TLB Miss
	if(!TLB_Hit){
		D_TLB_misses++;
		if(DPageTable[VPN].valid){ // PT Hit
			PT_Hit = true;
			D_PT_hits++;
			PPN = DPageTable[VPN].PPN;
			DMemory[PPN].last_cycle_used = Cycle;
		}
		else { // PT Miss
			D_PT_misses++;
			// Update DMemory
			if(!DMemoryIsFull()){ // DMemory Not Full
				for(int i=0; i < (int)DMemory.size(); i++){
					if(!DMemory[i].valid){
						DMemory[i].valid = true;
						DMemory[i].last_cycle_used = Cycle;
						PPN = i;
						break;
					}
				}
				// Update DTLB
				UpdateDTLB();
				// Update DPT
				UpdateDPT();
			}
			else { // DMemory Is Full
				int Min = 1000000, Index = 0;
				for(int i=0; i < (int)DMemory.size(); i++){
					if(DMemory[i].last_cycle_used < Min){
						Min = DMemory[i].last_cycle_used;
						Index = i;
					}
				}
				DMemory[Index].valid = true;
				DMemory[Index].last_cycle_used = Cycle;
				PPN = Index;
				// Clear DTLB
				for(int i=0; i < (int)DTLB.size(); i++){
					if(DTLB[i].valid && DTLB[i].PPN == PPN){
						DTLB[i].valid = false;
					}
				}
				// Clear DPT
				for(int i=0; i < (int)DPageTable.size(); i++){
					if(DPageTable[i].valid && DPageTable[i].PPN == PPN){
						DPageTable[i].valid = false;
					}
				}
				// Clear DCache
				for(int i=0; i < (int)DCache.size(); i++){
					for(int j=0; j < (int)DCache[i].size(); j++){
						if(DCache[i][j].valid){
							int tempPA = 0, tempPPN = 0;
							tempPA = tempPA | (DCache[i][j].tag << (32 - TagBits));
							tempPA = tempPA | (i << (32 - TagBits - IndexBits));
							tempPA = tempPA | j;
							tempPPN = tempPA / D_Memory_pagesize;
							if(tempPPN == PPN){
								DCache[i][j].valid = false;
								DCache[i][j].MRU = false;
							}
						}
					}
				}
				// Update DTLB
				UpdateDTLB();
				// Update DPT
				UpdateDPT();
			}
		}
	}
	// Calculate PA
	PA = PPN * D_Memory_pagesize + (VA % D_Memory_pagesize);

	// Search Cache
	int tag = (PA/D_cache_blocksize) / (int)DCache.size();
	int index = (PA/D_cache_blocksize) % (int)DCache.size();
	for(int i=0; i < (int)DCache[index].size(); i++){
		if(DCache[index][i].valid){
			if(DCache[index][i].tag == tag){ // DCache Hit
				D_cache_hits++;
				if(DCache[index].size()!=1){
					DCache[index][i].MRU = true;
				}
				Cache_Hit = true;
				break;
			}
		}
	}	
	// DCache Miss
	if(!Cache_Hit){
		D_cache_misses++;
		UpdateDCache(index, tag);
	}
}

void Calculate_CMP(int VA, bool isI){
	if(isI){ I_Hit_Misses(VA); }
	else { D_Hit_Misses(VA); }
}
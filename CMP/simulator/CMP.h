/***************************************************

	File: CMP.h

	Author: PinYo

***************************************************/

#ifndef CMP_h
#define CMP_h

class TLB_entry{
public:
	int VPN, PPN, last_cycle_used;
	bool valid;
	TLB_entry(){
		VPN = PPN = last_cycle_used = 0;
		valid = false;
	}
};

class PT_entry{
public:
	int PPN;
	bool valid;
	PT_entry(){
		PPN = 0;
		valid = false;
	}
};

class Cache_entry{
public:
	bool valid, MRU;
	int tag;
	Cache_entry(){
		valid = MRU = false;
		tag = 0;
	}
};

class Memory_entry{
public:
	int last_cycle_used;
	bool valid;
	Memory_entry(){
		last_cycle_used = 0;
		valid = false;
	}
};

void Calculate_CMP(int VA, bool isI);

#endif
/***************************************************

	File: Processor.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <vector>
#include "Instruction.h"
#include "Binary2Assembly.h"
#include "CMP.h"

using namespace std;

int Cycle = 0;
int I_Memory_size, D_Memory_size, I_Memory_pagesize, D_Memory_pagesize, I_cache_size, I_cache_blocksize, I_cache_nway, D_cache_size, D_cache_blocksize, D_cache_nway;
int I_cache_hits, I_cache_misses, D_cache_hits, D_cache_misses, I_TLB_hits, I_TLB_misses, D_TLB_hits, D_TLB_misses, I_PT_hits, I_PT_misses, D_PT_hits, D_PT_misses;
int Address[1024];
map< int,char > Memory;
int reg[32], PC;
bool Halt;

vector<TLB_entry> ITLB, DTLB;
vector<PT_entry> IPageTable, DPageTable;
vector<vector<Cache_entry>> ICache, DCache;
vector<Memory_entry> IMemory, DMemory;

void Initialize(){
	for(int i=0; i<32; i++)
		reg[i] = 0;
	for(int i=0; i<1024; i++)
		Address[i] = 0;
	I_Memory_size = 64;
	D_Memory_size = 32;
	I_Memory_pagesize = 8;
	D_Memory_pagesize = 16;
	I_cache_size = 16;
	I_cache_blocksize = 4;
	I_cache_nway = 4;
	D_cache_size = 16;
	D_cache_blocksize = 4;
	D_cache_nway = 1;

	I_cache_hits = 0;
	I_cache_misses = 0;
	D_cache_hits = 0;
	D_cache_misses = 0;
	I_TLB_hits = 0;
	I_TLB_misses = 0;
	D_TLB_hits = 0;
	D_TLB_misses = 0;
	I_PT_hits = 0;
	I_PT_misses = 0;
	D_PT_hits = 0;
	D_PT_misses = 0;
}

void PrintCycle(ofstream &fout){
	fout << "cycle " << dec << Cycle++ << endl;
	for(int i=0; i<32; i++){
		fout << "$" << setw(2) << setfill('0') << dec << i;
		fout << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg[i] << endl;
	}
	fout << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << PC << endl << endl << endl;
}

void PrintHitMiss(ofstream &HMout){
	// Print Cache
	HMout << "ICache :" << endl;
	HMout << "# hits: " << I_cache_hits << endl;
	HMout << "# misses: " << I_cache_misses << endl << endl;
	HMout << "DCache :" << endl;
	HMout << "# hits: " << D_cache_hits << endl;
	HMout << "# misses: " << D_cache_misses << endl << endl;
	// Print TLB
	HMout << "ITLB :" << endl;
	HMout << "# hits: " << I_TLB_hits << endl;
	HMout << "# misses: " << I_TLB_misses << endl << endl;
	HMout << "DTLB :" << endl;
	HMout << "# hits: " << D_TLB_hits << endl;
	HMout << "# misses: " << D_TLB_misses << endl << endl;
	// Print PT
	HMout << "IPageTable :" << endl;
	HMout << "# hits: " << I_PT_hits << endl;
	HMout << "# misses: " << I_PT_misses << endl << endl;
	HMout << "DPageTable :" << endl;
	HMout << "# hits: " << D_PT_hits << endl;
	HMout << "# misses: " << D_PT_misses << endl << endl;
}

void InitializeCMP(){
	IPageTable.resize(1024/I_Memory_pagesize);
	DPageTable.resize(1024/D_Memory_pagesize);
	ITLB.resize(IPageTable.size()/4);
	DTLB.resize(DPageTable.size()/4);
	ICache.resize(I_cache_size/I_cache_nway/I_cache_blocksize);
	for(int i=0; i<(int)ICache.size(); i++){
		ICache[i].resize(I_cache_nway);
	}
	DCache.resize(D_cache_size/D_cache_nway/D_cache_blocksize);
	for(int i=0; i<(int)DCache.size(); i++){
		DCache[i].resize(D_cache_nway);
	}
	IMemory.resize(I_Memory_size/I_Memory_pagesize);
	DMemory.resize(D_Memory_size/D_Memory_pagesize);
}

int main(int argc, char* argv[]){
	char ch;
	int Word = 0;

	// Initialize register;
	Initialize();
	if(argc > 1){
		I_Memory_size = atoi(argv[1]);
		D_Memory_size = atoi(argv[2]);
		I_Memory_pagesize = atoi(argv[3]);
		D_Memory_pagesize = atoi(argv[4]);
		I_cache_size = atoi(argv[5]);
		I_cache_blocksize = atoi(argv[6]);
		I_cache_nway = atoi(argv[7]);
		D_cache_size = atoi(argv[8]);
		D_cache_blocksize = atoi(argv[9]);
		D_cache_nway = atoi(argv[10]);
	}

	ofstream fout("snapshot.rpt", ios::out);
	ofstream HMout("report.rpt", ios::out);

	// Read iimage.bin
	ifstream fin("iimage.bin", ios::in | ios::binary);
	if(!fin){
		cout << "Error to load 'iimage.bin'!\n";
		return 0;
	}
	// Read PC
	for(int i=0; i<4; i++){
		fin.get(ch);
		Word = (Word << 8) | (unsigned char)ch;
	}
	PC = Word;
	// Read numbers of words
	for(int i=0; i<4; i++){
		fin.get(ch);
		Word = (Word << 8) | (unsigned char)ch;
	}
	int Num = Word;
	// Read Instructions
	for(int i=0; i<Num; i++){
		for(int j=0; j<4; j++){
			fin.get(ch);
			Word = (Word << 8) | (unsigned char)ch;
		}
		Address[PC+i*4] = Word;
	}

	fin.close();

	// Read dimage.bin
	fin.open("dimage.bin", ios::in | ios::binary);
	// Read $sp
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	reg[29] = Word;
	// Numbers of words
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	int NumbersOfWords = Word;
	for(int i=0; i<NumbersOfWords*4; i++){
		fin.get(ch);
		Memory[i] = ch;
	}

	PrintCycle(fout);
	Halt = false;
	InitializeCMP();

	while(!Halt){
		Calculate_CMP(PC, true);
		Binary2Assembly();
		cout << "---------------Cycle: " << Cycle << endl;
		for(int i=0; i<ICache.size(); i++){
			cout << "Set: " << i << endl;
			for(int j=0; j<ICache[i].size(); j++){
				cout << "Valid: " << ICache[i][j].valid;
				cout << "  Tag: " << ICache[i][j].tag;
				cout << "  MRU: " << ICache[i][j].MRU << endl;
			}
		}
		cout << "---------------------" << endl;

		if(Halt) break;
		PrintCycle(fout);
	}
	PrintHitMiss(HMout);

	return 0;
}
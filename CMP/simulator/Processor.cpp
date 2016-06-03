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
int Address[1024];
map< int,char > Memory;
int reg[32], PC;
bool Halt;

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
}

void PrintCycle(ofstream &fout){
	fout << "cycle " << dec << Cycle++ << endl;
	for(int i=0; i<32; i++){
		fout << "$" << setw(2) << setfill('0') << dec << i;
		fout << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg[i] << endl;
	}
	fout << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << PC << endl << endl << endl;
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

	while(!Halt){
		Calculate_CMP();
		Binary2Assembly();
		PrintCycle(fout);
	}

	return 0;
}
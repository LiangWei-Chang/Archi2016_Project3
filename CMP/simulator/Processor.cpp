/***************************************************

	File: Processor.cpp

	Author: PinYo

***************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>
#include "GlobalVar.h"
#include "Binary2Assembly.h"

using namespace std;

int Cycle = 0;

void Initialize(){
	for(int i=0; i<32; i++)
		Global::reg[i] = 0;
	for(int i=0; i<1024; i++)
		Global::Address[i] = 0;
}

void PrintCycle(ofstream &fout){
	fout << "cycle " << dec << Cycle++ << endl;
	for(int i=0; i<32; i++){
		fout << "$" << setw(2) << setfill('0') << dec << i;
		fout << ": 0x" << setw(8) << setfill('0') << hex << uppercase << Global::reg[i] << endl;
	}
	fout << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << Global::PC << endl << endl << endl;
}

int main(){
	char ch;
	int Word = 0;

	// Initialize register;
	Initialize();

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
	Global::PC = Word;
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
		Global::Address[Global::PC+i*4] = Word;
	}

	fin.close();

	// Read dimage.bin
	fin.open("dimage.bin", ios::in | ios::binary);
	// Read $sp
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	Global::reg[29] = Word;
	// Numbers of words
	for(int i=4; i>0; i--){
		fin.get(ch);
		Word = (Word << 8) + (unsigned char)ch;
	}
	int NumbersOfWords = Word;
	for(int i=0; i<NumbersOfWords*4; i++){
		fin.get(ch);
		Global::Memory[i] = ch;
	}

	PrintCycle(fout);
	Global::Halt = false;

	while(!Global::Halt){
		Binary2Assembly();
		PrintCycle(fout);
	}

	return 0;
}
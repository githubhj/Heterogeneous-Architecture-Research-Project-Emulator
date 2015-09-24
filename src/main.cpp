/*
 * main.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: harshitjain
 */

#include <iostream>
#include <fstream>
#include "Instruction.h"
#include "ReadBinary.h"
#include <cassert>
#include <vector>
#include <limits>

using namespace std;


typedef union _readbuff{

}readbuff;

int main(int argc, char* argv[]){

	//Check for arguments
	assert(argc > 2);
	std::vector<std::string> args;
	std::copy(argv+1, argv + argc, std::back_inserter(args));
	bool useDefault=false;
	if(args.size()==2){
		useDefault = true;
	}

	int instructionLength = DEFAULT_IL;
	int gprNum = DEFAULT_GN;
	int pregNum = DEFAULT_PN;
	int simdLanes = DEFAULT_SL;
	int warpNum = DEFAULT_WN;

	if(!useDefault){
		if(args.size()==7){
			instructionLength = stoi(args[2]);
			//cout << instructionLength << endl;
			gprNum = stoi(args[3]);
			pregNum	= stoi(args[4]);
			simdLanes = stoi(args[5]);
			warpNum = stoi(args[6]);
		}
	}

	ReadBinary fileHandler(args[0],instructionLength);



	//std::cout << fileHandler.binaryfileLength <<endl;
	unsigned long long instruction =0;


	for(int i = 0 ; i<fileHandler.binaryfileLength; i+=instructionLength){
		unsigned char* instPtr=(unsigned char*)fileHandler.getInstruction(i);
		instruction =0;
		for(int j=instructionLength-1;j>=0;j--){
			instruction = (instruction << CHAR_BIT) | (unsigned long long)instPtr[j];
		}
		cout << hex << instruction << endl;
	}



}


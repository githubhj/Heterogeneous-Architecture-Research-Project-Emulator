//============================================================================
// Name        : MemoryMap.cpp
// Author      : Harshit Jain
// Version     : 1.0
// Copyright   : It's protected under Beerware
// GT ID       : 903024992
// Class       : ECE 8823
// Assignment  : 3
// Description : MemoryMap class in C++, Ansi-style
//============================================================================

#include "MemoryMap.h"

MemoryMap::MemoryMap(std::string binaryFileName, uint64_t instSize) {
	std::ifstream binaryFile;
	binaryFile.open(binaryFileName,std::ios::binary|std::ios::in);
	binaryFile.seekg(0,binaryFile.end);
	binaryFileLength = binaryFile.tellg();
	binaryFile.seekg(0,binaryFile.beg);
	uint8_t* readBuff = new uint8_t;
    
	for(uint64_t j = 0 ; j< binaryFileLength; j++){
        binaryFile.read((char*)(readBuff), sizeof(uint8_t));
        if ((*readBuff)!=0) {
          memoryBuff[j] = *readBuff;
        }
	}
	delete[] readBuff;

	binaryFile.close();
}

void MemoryMap::printMemoryBuff(){
	std::cout << "Address:\tInstruction\t" << "\n";
	for(memoryBuff_t::iterator it = memoryBuff.begin() ; it != memoryBuff.end(); it++){
		std::cout << std::hex << it->first << "\t" << it->second << "\n";
	}
    
}


void MemoryMap::insert(uint64_t addr, uint64_t data){
	memoryBuff[addr] = data;
}

MemoryMap::~MemoryMap() {
	// TODO Auto-generated destructor stub
}


/*
 * MemoryMap.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: harshitjain
 */

#include "MemoryMap.h"

MemoryMap::MemoryMap(std::string binaryFileName, uint64_t instSize) {
	std::ifstream binaryFile;
	binaryFile.open(binaryFileName,std::ios::binary|std::ios::in);
	binaryFile.seekg(0,binaryFile.end);
	binaryFileLength = binaryFile.tellg();
	binaryFile.seekg(0,binaryFile.beg);
	uint8_t* readBuff = new uint8_t[instSize];
	uint64_t instAddr = 0;
	uint64_t instData  = 0;
	for(uint64_t j = 0 ; j< binaryFileLength; j+= instSize){
		binaryFile.read((char*)readBuff,instSize);
		for(int32_t i= instSize-1; i>=0; i--){
			instData = (instData << CHAR_BIT) | (uint64_t)readBuff[i];
		}
		if(instData){
			memoryBuff[instAddr] = instData;
		}
		instAddr+=instSize;
		instData =0;
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


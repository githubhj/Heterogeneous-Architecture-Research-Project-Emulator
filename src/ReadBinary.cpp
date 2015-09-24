/*
 * ReadBinary.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: harshitjain
 */

#include "ReadBinary.h"

ReadBinary::ReadBinary(std::string fileName, int instrLength) {
	// TODO Auto-generated constructor stub
	binaryFile.open(fileName.c_str(),std::ios::binary);
	binaryFile.seekg(0,binaryFile.end);
	binaryfileLength = binaryFile.tellg();
	binaryFile.seekg(0,binaryFile.beg);

	instructionLength = instrLength;
	readBuff = new char[binaryfileLength];
	instBuff = new char[instrLength];

	binaryFile.read(readBuff,sizeof(readBuff));

	std::ofstream outputFile("output.txt",std::ostream::binary);
	outputFile.open("w");
	outputFile.write(readBuff,binaryfileLength);
	outputFile.close();
	//binaryFile.close();
}

char* ReadBinary::getInstruction(unsigned long long address){

	if(address > binaryfileLength - instructionLength){
		return nullptr;
	}
	else{
		binaryFile.seekg(address,binaryFile.beg);
		binaryFile.read(instBuff,sizeof(instBuff));
		return instBuff;
	}

	/*
	if(binaryFile.is_open() && !binaryFile.eof()){
		binaryFile.read(readBuff,sizeof(readBuff));
		binaryFile.seekg(sizeof(readBuff),binaryFile.cur);
		return readBuff;
	}
	else{
		return nullptr;
	}*/

}

char* ReadBinary::getByte(unsigned long long address){
	if(address >= binaryfileLength){
		return nullptr;
	}
	else{
		return readBuff + address;
	}
}

bool ReadBinary::isEmpty(){
	return binaryFile.eof();
}
ReadBinary::~ReadBinary() {
	binaryFile.close();
	delete[] readBuff;
}


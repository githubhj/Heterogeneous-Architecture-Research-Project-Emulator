/*
 * ReadBinary.h
 *
 *  Created on: Sep 21, 2015
 *      Author: harshitjain
 */

#ifndef SRC_READBINARY_H_
#define SRC_READBINARY_H_

#include <iostream>
#include <string>
#include <fstream>

#define NULL nullptr;

class ReadBinary {
private:
	std::ifstream binaryFile;
	int instructionLength;
	char* readBuff;

	char* instBuff;
public:
	unsigned long long binaryfileLength;
	ReadBinary(std::string fileName, int instrLength);
	char* getInstruction(unsigned long long address);
	char* getByte(unsigned long long address);
	bool isEmpty();
	virtual ~ReadBinary();
};

#endif /* SRC_READBINARY_H_ */

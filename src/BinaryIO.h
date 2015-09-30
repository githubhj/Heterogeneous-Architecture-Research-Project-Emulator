/*
 * BinaryIO.h
 *
 *  Created on: Sep 24, 2015
 *      Author: harshitjain
 */

#ifndef BinaryIO_H_
#define BinaryIO_H_
#include <iostream>
#include <fstream>
#include <cstdint>


class BinaryIO {
private:
	uint8_t* inBuff;
	uint8_t* outBuff;

	uint32_t readLength;
	uint32_t writeLength;

	uint64_t presentFilePos;

	std::fstream binaryFile;
	std::string fileName;
public:
	//General Constructor
	BinaryIO(uint32_t rLength, uint32_t wLength, std::string file);

	//Copy constructor
	BinaryIO(const BinaryIO& other);

	//Assignment Operator
	BinaryIO& operator=(const BinaryIO& rhs);

	//Get a pointer to data of length readLength
	uint8_t* getData(uint64_t addr);

	//Write data at an address or wrriteLength
	bool putData(uint64_t addr, uint8_t* dataPtr);

	uint64_t fileLength;
	virtual ~BinaryIO();
};

#endif /* BinaryIO_H_ */

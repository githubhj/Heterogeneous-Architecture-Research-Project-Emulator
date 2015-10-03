/*
 * MemoryMap.h
 *
 *  Created on: Sep 29, 2015
 *      Author: harshitjain
 */

#ifndef SRC_MEMORYMAP_H_
#define SRC_MEMORYMAP_H_

#include <iostream>
#include <limits>
#include <fstream>
#include <cstdint>
#include <map>

typedef std::map<uint64_t,uint8_t> memoryBuff_t;


class MemoryMap {

public:
	MemoryMap(){};
	MemoryMap(std::string,uint64_t);
	void insert(uint64_t addr, uint64_t data);
	memoryBuff_t memoryBuff;
	uint64_t binaryFileLength;
	void printMemoryBuff();
	virtual ~MemoryMap();
};

#endif /* SRC_MEMORYMAP_H_ */

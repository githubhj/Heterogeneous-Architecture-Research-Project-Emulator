/*
 * Instruction.h
 *
 *  Created on: Sep 22, 2015
 *      Author: harshitjain
 */

#ifndef SRC_INSTRUCTION_H_
#define SRC_INSTRUCTION_H_

#include <iostream>
#include <string>

typedef enum {
	AC_NONE, AC_2REG, AC_2IMM, AC_3REG, AC_3PREG, AC_3IMM, AC_3REGSRC, AC_1IMM,
	AC_1REG, AC_3IMMSRC, AC_PREG_REG, AC_2PREG
} ArgumentClass_t;

#define DEFAULT_IL 4
#define DEFAULT_GN 8
#define DEFAULT_PN 8
#define DEFAULT_SL 1
#define DEFAULT_WN 1


class Instruction {
private:
	//In Bytes
	int instLength;
	//In Numbners
	int gprNum;
	int pregNum;
	int simdLanes;
	int warNum;

	//Core registers
	unsigned long long programCounter;
	unsigned char* gprRegisters;
	unsigned char* pregRegisters;

	//Predicate bit necessary for execution
	bool predicateBit;

public:
	Instruction();
	~Instruction();
};

#endif /* SRC_INSTRUCTION_H_ */

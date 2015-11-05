//============================================================================
// Name        : harp_emulator_driver.cpp
// Author      : Harshit Jain
// Version     : 1.0
// Copyright   : It's protected under Beerware
// GT ID       : 903024992
// Class       : ECE 8823
// Assignment  : 4
// Description : Harp Emulator in C++, Ansi-style
//============================================================================

#include <iostream>
#include "harp_emulator_driver.h"
#include <cassert>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <istream>
#include <algorithm>

using namespace std;

/*bool loadOpcodeToInstr
 params:    ArchSpec_t*.
 fucntion:  Loads archSpec vector with instrction oopcode to
            argument mapping.
 */

bool loadOpcodeToInstr(ArchSpec_t* archSpec){
	if(archSpec == nullptr){
		return false;
	}
	else{
		//Setup instruction to argument class mapping
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//nop
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//di
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//ei
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REGSRC);		//tlbadd
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//tlbflush
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//neg
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//not
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//and
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//or
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//xor
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//add
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//sub
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//mul
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//div
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//mod
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//shl
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//shr
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//andi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//ori
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//xori
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//addi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//subi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//muli
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//divi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//modi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//shli
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//shri
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2IMM);		//jali
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//jalr
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1IMM);		//jmpi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//jmpr
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//clone
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//jalis
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//jalrs
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//jmprt
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMM);		//ld
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3IMMSRC);		//st
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2IMM);		//ldi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_PREG_REG);	//rtop
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3PREG);		//andp
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3PREG);		//orp
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3PREG);		//xorp
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2PREG);		//notp
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_PREG_REG);	//isneg
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_PREG_REG);	//iszero
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//halt
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//trap
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//jmpru
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//skep
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//reti
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_1REG);		//tlbrm
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//itof
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//ftoi
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//fadd
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//fsub
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//fmul
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//fdiv
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//fneg
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_3REG);		//wspawn
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//split
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_NONE);		//join
		archSpec->opcodeToArgument.push_back(ArgumentEnum::AC_2REG);		//bar
		return true;
	}
}

/*
 main(): Driver code
 */
int main(int argc, char* argv[]) {

	//Check if arguments for input and output files are present or not
	assert(argc > 2);

	//Take default architecture specifications
	uint32_t instructionLength  = DEFAULT_IL;
	uint32_t generalPurposeReg 	= DEFAULT_GN;
	uint32_t predicateReg		= DEFAULT_PN;
	uint32_t simdLanes			= DEFAULT_SL;
	uint32_t warpNum			= DEFAULT_WN;

	//Copy arguments
	std::vector<std::string> args;
	std::copy(argv+1, argv + argc, std::back_inserter(args));

	std::string inputFile 	= args[0];
	std::string outputFile 	= args[1];

	//If new architecture specifications are present
	if(args.size() == 7){

		//Instruction Length
		instructionLength 	= 	stoi(args[2]);
		if((instructionLength !=4) && (instructionLength !=8)){
			std::cout << "Instruction Length should be either 4 or 8.\n";
			exit(1);
		}

		//General Purpose Length
		generalPurposeReg	=	stoi(args[3]);
        if(generalPurposeReg==0){
            std::cout << "General Purpose registers cannot be 0.\n";
            exit(1);
        }
        else if(generalPurposeReg&(generalPurposeReg-1)){
			std::cout << "(Warning:)\tGeneral purpose register number not a power of 2.\n";
            uint32_t tempVar = generalPurposeReg;
            uint32_t powerCount=1;
            while (tempVar >>= 1) {
                powerCount++;
            }
            generalPurposeReg = 1<<powerCount;
            std::cout << "(Warning:)\tTaking general purpose register a power of two: " << generalPurposeReg << ".\n";
		}

		//Predicate Register Length
		predicateReg		=	stoi(args[4]);
        if(predicateReg==0){
            std::cout << "Predicate registers cannot be 0.\n";
            exit(1);
        }
        else if(predicateReg&(predicateReg-1)){
            std::cout << "(Warning:)\tPredicate register number not a power of 2.\n";
            uint32_t tempVar = predicateReg;
            uint32_t powerCount=1;
            while (tempVar >>= 1) {
                powerCount++;
            }
            predicateReg = 1<<powerCount;
            std::cout << "(Warning:)\tTaking predicate register a power of two: " << predicateReg << ".\n";
        }

		//Simd Lanes
		simdLanes			=	stoi(args[5]);
		if(simdLanes==0){
			std::cout << "SIMD Lanes cannot be 0.\n";
			exit(1);
		}

		//Warp Numbers
		warpNum				=	stoi(args[6]);
		if(warpNum==0){
			std::cout << "Warp number cannot be 0.\n";
			exit(1);
		}
	}

	//Specify architecture details
	std::cout << "\n\n---------Harp Emulator Specifications, Implemented as per harp ISA--------\n";
	std::cout << "Instruction Length(in bytes,CONFIGURABLE)\t:\t" 		<< instructionLength<< "\n";
	std::cout << "General Purpose Register Number(CONFIGURABLE)\t:\t" 	<< generalPurposeReg<< "\n";
	std::cout << "Predicate Register Number(CONFIGURABLE)\t\t:\t" 		<< predicateReg		<< "\n";
	std::cout << "SIMD Lanes(CONFIGURABLE)\t\t\t:\t"					<< simdLanes		<< "\n";
	std::cout << "Warp number(CONFIGURABLE)\t\t\t:\t"					<< warpNum			<< "\n";
	std::cout << "Opcode Length(FIXED)\t\t\t\t:\t"						<< "6"			<< "\n\n";
	std::cout << "Help:\t ./harp_emulator <input binary(Required)> <output binary(Required)> <instruction width> <GPR Number> <PREG Number> <SIMD Lanes> <Warp Number>" << "\n";

	//Configure Architecture specification
	ArchSpec_t* simdSpecPtr 	= new ArchSpec_t;
	simdSpecPtr->gprNum 	= generalPurposeReg;
	simdSpecPtr->instLength	= instructionLength;
	simdSpecPtr->pregNum	= predicateReg;
    //Hard coded
	simdSpecPtr->writeAddr	= 0x80000000;
	loadOpcodeToInstr(simdSpecPtr);

	//Get bit length required for GPR and PREG
	uint32_t gprBitLength 	= log2((double)generalPurposeReg);

	uint32_t pregBitLength 	= log2((double)predicateReg);


	simdSpecPtr->gprBitLength  = gprBitLength;
	simdSpecPtr->pregBitLength = pregBitLength;
    simdSpecPtr->simdLaneSize  = simdLanes;
    simdSpecPtr->warpSize      = warpNum;

	//Set opcode and Predicate fields
	if(instructionLength == 4){
		simdSpecPtr->predicate.position 	= 32;
		simdSpecPtr->predicate.length		= 1;
		simdSpecPtr->pregRegField.position	= 32 - 1;
		simdSpecPtr->pregRegField.length	= pregBitLength;
		simdSpecPtr->opcode.position		= 32 - 1 - pregBitLength;
		simdSpecPtr->opcode.length			= 6;
	}

	else{
		simdSpecPtr->predicate.position 	= 64;
		simdSpecPtr->predicate.length		= 1;
		simdSpecPtr->pregRegField.position	= 64 - 1;
		simdSpecPtr->pregRegField.length	= pregBitLength;
		simdSpecPtr->opcode.position		= 64 - 1 - pregBitLength;
		simdSpecPtr->opcode.length			= 6;
	}

	//Configure Memory Map
	MemoryMap* memoryMap = new MemoryMap(inputFile,instructionLength);
    
    //Initialize output memory
    threadOutputMemory_t threadOutputMemory;
    
	//Initialize GPU
    std::vector< std::shared_ptr<SimdCoreBase> > GPU(simdLanes);
    //If instruction width is 4
	if(instructionLength == 4){
        GPU[0].reset(new SimdCore<uint32_t>(simdSpecPtr,memoryMap));
	}
    //Else it is 8
	else{
        GPU[0].reset(new SimdCore<uint64_t>(simdSpecPtr,memoryMap));
	}
    
    //GPU[0]->reset();
    GPU[0]->start(false);
    
    
    //Write to the output file
    ofstream outFile;
    outFile.open(outputFile);
    outFile << GPU[0]->getOutputData();
    outFile.close();
    
    //Delete members on heap
    delete memoryMap;
    delete simdSpecPtr;

	return 0;
}

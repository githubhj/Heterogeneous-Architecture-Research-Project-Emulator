//============================================================================
// Name        : harp_emulator.cpp
// Author      : Harshit Jain
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "BinaryIO.h"
#include "harp_emulator_driver.h"
#include <cassert>
#include <vector>
#include <cstdlib>
#include "SimdCore.h"
#include <cmath>
using namespace std;


int main(int argc, char* argv[]) {

	//Check if arguments for input and output files are present or not
	assert(argc > 2);

	//Take default architecture specifications
	uint64_t instructionLength  = DEFAULT_IL;
	uint64_t generalPurposeReg 	= DEFAULT_GN;
	uint64_t predicateReg		= DEFAULT_PN;
	uint64_t simdLanes			= DEFAULT_SL;
	uint64_t warpNum			= DEFAULT_WN;

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
		if((generalPurposeReg==0) || (generalPurposeReg&(generalPurposeReg-1))){
			std::cout << "General purpose register number should be power of 2.\n";
			exit(1);
		}

		//Predicate Register Length
		predicateReg		=	stoi(args[4]);
		if((predicateReg==0) || (predicateReg&(predicateReg-1))){
			std::cout << "Predicate register number should be power of 2.\n";
			exit(1);
		}

		//Simd Lanes
		simdLanes			=	stoi(args[5]);
		if((simdLanes==0) || (simdLanes&(simdLanes-1))){
			std::cout << "SIMD Lanes should be power of 2.\n";
			exit(1);
		}

		//Warp Numbers
		warpNum				=	stoi(args[6]);
		if((simdLanes==0) || (simdLanes&(simdLanes-1))){
			std::cout << "Warp number should be power of 2.\n";
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
	ArchSpec* simdSpecPtr 	= new ArchSpec;
	simdSpecPtr->gprNum 	= generalPurposeReg;
	simdSpecPtr->instLength	= instructionLength;
	simdSpecPtr->pregNum	= predicateReg;
	simdSpecPtr->simdLanes	= simdLanes;
	simdSpecPtr->warpNum	= warpNum;
	simdSpecPtr->writeAddr	= 0x80000000;

	//Get bit length required for GPR and PREG
	uint32_t gprBitLength 	= log2((double)generalPurposeReg);
	cout << gprBitLength << "\n";
	uint32_t pregBitLength 	= log2((double)predicateReg);
	cout << pregBitLength << "\n";

	simdSpecPtr->gprBitLength  = gprBitLength;
	simdSpecPtr->pregBitLength = pregBitLength;

	//Set opcode and Predicate fields
	if(instructionLength == 4){
		simdSpecPtr->predicate.position 	= 31;
		simdSpecPtr->predicate.length		= 1;
		simdSpecPtr->opcode.position		= 31 - pregBitLength -1;
		simdSpecPtr->opcode.length			= 6;
	}

	else{
		simdSpecPtr->predicate.position 	= 63;
		simdSpecPtr->predicate.length		= 1;
		simdSpecPtr->opcode.position		= 63 - pregBitLength -1;
		simdSpecPtr->opcode.length			= 6;
	}

	//Initialize GPU
	//SimdCore* GPU = new SimdCore[simdLanes](inputFile,outputFile,simdSpecPtr);
	SimdCore GPU(inputFile,outputFile,simdSpecPtr);
	//GPU.printBinary();
	//GPU.printFileLength();
	GPU.load();
	GPU.start();
	uint64_t inst = 0x0158FFFF;
	//GPU.execute(inst);
	for(uint32_t i = 0 ; i < simdLanes; i++){

		//Blocking call

		//GPU[i].start();

	}

	return 0;
}

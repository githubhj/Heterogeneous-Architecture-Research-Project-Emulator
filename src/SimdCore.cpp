/*
 * SimdCore.cpp
 *
 *  Created on: Sep 29, 2015
 *      Author: harshitjain
 */

#include "SimdCore.h"

template<typename T>
SimdCore<T>::SimdCore(ArchSpec_t* simdSpec_i, MemoryMap* memoryMap_i){
	simdSpec 		= simdSpec_i;
	memoryMap 		= memoryMap_i;
	gprFile			= new T[simdSpec->gprNum];
	pregFile		= new T[simdSpec->pregNum];
	programCounter	= 0;
};

template<typename T>
void SimdCore<T>::start(bool debug){
	programCounter =0;

	bool executeNext = true;
    T tempAddr;
	while(executeNext){
        if (memoryMap->memoryBuff.find((unsigned long long)programCounter) == memoryMap->memoryBuff.end()) {
            break;
        }
        tempAddr = (T)memoryMap->memoryBuff[programCounter];
		executeNext = this->execute(tempAddr, debug);
	}
}

template<typename T>
bool SimdCore<T>::execute(T instruction, bool debug){
	uint32_t opcodeValue =0;
	T* gprInput1 	= nullptr;
	T* gprInput2 	= nullptr;
	T* gprOut		= nullptr;
	T* pregIn1		= nullptr;
	T* pregIn2		= nullptr;
	T* pregOut		= nullptr;
	T immediate		= 0;
	ArgumentEnum_t	opcodeArg;

	bool predicateBit = (instruction >> ((simdSpec->predicate.position)-1));

	//Predicated check
	if(predicateBit){
		uint32_t predRegNum =  ((instruction << (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (simdSpec->pregRegField.position - simdSpec->pregRegField.length);
		if(pregFile[predRegNum]==0){
			programCounter += simdSpec->instLength;
			return true;
		}
	}

	//Normal Execution

	//Get Opcode
	uint32_t opcodeBitsBefore 	= sizeof(T)*CHAR_BIT-simdSpec->opcode.position;
	uint32_t opcodeBitsAfter 	= simdSpec->opcode.position - simdSpec->opcode.length;
	opcodeValue = ((instruction << opcodeBitsBefore) >> opcodeBitsBefore) >> opcodeBitsAfter;

	//Get Arguments
	opcodeArg = simdSpec->opcodeToArgument[opcodeValue];
	std::cout << std::hex << opcodeValue << "\n";
	std::cout << std::hex << opcodeArg << "\n";

	//Get opcode with args
	uint32_t regShiftLeftVal = sizeof(T)*CHAR_BIT - (simdSpec->opcode.position - simdSpec->opcode.length);
	T opcodeWithArgs = (instruction << regShiftLeftVal) >> regShiftLeftVal;

	uint32_t rightShiftVal1	=0;
	uint32_t rightShiftVal2	=0;
	uint32_t rightShiftVal3	=0;
	uint32_t leftShiftVal1  =0;
	uint32_t leftShiftVal2  =0;
	uint32_t gprNum1		=0;
	uint32_t gprNum2		=0;
	uint32_t gprNum3		=0;
	uint32_t pregNum1		=0;
	uint32_t pregNum2		=0;
    uint32_t pregNum3		=0;
	T immVal 				=0;



	switch(opcodeArg){

			case(ArgumentEnum::AC_NONE):{
				break;
			}
			case(ArgumentEnum::AC_2REG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
				gprOut 					= gprFile + gprNum1;
				gprInput1 				= gprFile + gprNum2;
				break;
			}
			case(ArgumentEnum::AC_2IMM):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				immVal			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1);
				gprOut 			= gprFile + gprNum1;
				immediate 		= immVal;
                if (immediate >> (rightShiftVal1-1)) {
                    T mask = 0;
                    uint32_t count = rightShiftVal1;
                    while (count) {
                        mask = (mask << 1) | 1;
                        count--;
                    }
                    mask = ~mask;
                    immediate |= mask;
                }
				break;
			}
			case(ArgumentEnum::AC_3REG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				rightShiftVal3	= rightShiftVal2 - simdSpec->gprBitLength;
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
				leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
				gprNum3 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
				gprOut 					= gprFile + gprNum1;
				gprInput1 				= gprFile + gprNum2;
				gprInput2				= gprFile + gprNum3;
				break;
			}
            
            case (ArgumentEnum::AC_3PREG):{
                rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
                rightShiftVal2 	= rightShiftVal1 - simdSpec->pregBitLength;
                rightShiftVal3	= rightShiftVal2 - simdSpec->pregBitLength;
                pregNum1 		= opcodeWithArgs >> rightShiftVal1;
                leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
                pregNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
                leftShiftVal2  	= leftShiftVal1 + simdSpec->pregBitLength;
                pregNum3 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
                pregOut 				= pregFile + pregNum1;
                pregIn1 				= pregFile + pregNum2;
                pregIn2                 = pregFile + pregNum3;
                break;
            }

			case(ArgumentEnum::AC_3IMM):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
				leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
				immVal 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);
				gprOut 			= gprFile + gprNum1;
				gprInput1 		= gprFile + gprNum2;
				immediate		= immVal;
                if (immediate >> (rightShiftVal2-1)) {
                    T mask = 0;
                    uint32_t count = rightShiftVal2;
                    while (count) {
                        mask = (mask << 1) | 1;
                        count--;
                    }
                    mask = ~mask;
                    immediate |= mask;
                }
				break;
			}

			case(ArgumentEnum::AC_3REGSRC):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				rightShiftVal3	= rightShiftVal2 - simdSpec->gprBitLength;
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
				leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
				gprNum3 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
				gprOut 			= gprFile + gprNum1;
				gprInput1 		= gprFile + gprNum2;
				gprInput2		= gprFile + gprNum3;
				break;
			}

			case(ArgumentEnum::AC_1IMM):{
                immediate = opcodeWithArgs ;
                rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length);
                if (immediate >> (rightShiftVal1-1)) {
                    T mask = 0;
                    uint32_t count = rightShiftVal1;
                    while (count) {
                        mask = (mask << 1) | 1;
                        count--;
                    }
                    mask = ~mask;
                    immediate |= mask;
                }
				break;
			}

			case(ArgumentEnum::AC_1REG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				gprInput1 		= gprFile + gprNum1;
				break;
			}

			case(ArgumentEnum::AC_3IMMSRC):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum2			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
				leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
				immVal 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);
				gprInput1 		= gprFile + gprNum1;
				gprInput2		= gprFile + gprNum2;
				immediate		= immVal;
                if (immediate >> (rightShiftVal2-1)) {
                    T mask = 0;
                    uint32_t count = rightShiftVal2;
                    while (count) {
                        mask = (mask << 1) | 1;
                        count--;
                    }
                    mask = ~mask;
                    immediate |= mask;
                }
				break;
			}

			case(ArgumentEnum::AC_PREG_REG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
				pregNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				gprNum1			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;

				pregOut					= pregFile + pregNum1;
				gprInput1 				= gprFile  + gprNum1;
				break;
			}

			case(ArgumentEnum::AC_2PREG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->pregBitLength;
				pregNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
				pregNum2		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;

				pregOut			= pregFile 	+ pregNum1;
				pregIn1 		= pregFile  + pregNum2;
				break;
			}
			default:{
				return false;
				break;
			}
		}
    
   

		if(debug){
			std::cout << "----------------------------------------------------------" << "\n";
			std::cout << std::hex <<	"Instruction:\t" << instruction << "\n";
			std::cout << std::hex <<	"Programcounter:\t" << programCounter << "\n";
			std::cout << std::hex <<	"Opcode:\t" << opcodeValue << "\n";
			std::cout << std::hex << 	"GPR Values:" << "GPR1: " << gprNum1 <<  " GPR2: " << gprNum2 << " GPR3: " << gprNum3 << "\n";
			std::cout << std::hex << 	"PREG Values:" << "PREG1: " << pregNum1 <<  "PREG2: " << pregNum2 << "\n";
			std::cout << std::hex << 	"Immediate Value:" << immVal;
			std::cout << "----------------------------------------------------------" << "\n";
		}

		switch(opcodeValue){
			//NOP
			case(0):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//di
			case(1):{
				programCounter +=  simdSpec->instLength;
				return true;
				break;
			}
			//ei
			case(2):{
				programCounter +=  simdSpec->instLength;
				return true;
				break;
			}
			//tlbadd
			case(3):{
				programCounter +=  simdSpec->instLength;
				return true;
				break;
			}
			//tlbflush
			case(4):{
				return true;
				break;
			}
			//neg
			case(5):{
				if(gprInput1 && gprOut){
					*gprOut = -(*gprInput1);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//not
			case(6):{
				if(gprInput1 && gprOut){
					*gprOut = ~(*gprInput1);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//and
			case(7):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) & (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//or
			case(8):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) | (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//xor
			case(9):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) ^ (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//add
			case(10):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) + (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//sub
			case(11):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) - (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//mul
			case(12):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) * (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//div
			case(13):{
				if(gprInput1 && gprInput2 && gprOut){
					if((*gprInput2)!=0){
						*gprOut = (*gprInput1) / (*gprInput2);
						programCounter +=  simdSpec->instLength;
						return true;
					}
					else{
						if(simdSpec->gprBitLength == 4)
							*gprOut = UINT_MAX;
						else
							*gprOut = ULONG_MAX;
						return false;
					}

				}
				break;
			}
			//mod
			case(14):{
				if(gprInput1 && gprInput2 && gprOut){
					if((*gprInput2)!=0){
						*gprOut = (*gprInput1) % (*gprInput2);
						programCounter +=  simdSpec->instLength;
						return true;
					}
					else{
						if(simdSpec->gprBitLength == 4)
							*gprOut = UINT_MAX;
						else
							*gprOut = ULONG_MAX;
						return false;
					}
				}
				break;
			}
			//shl
			case(15):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) << (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//shr
			case(16):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) >> (*gprInput2);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//andi
			case(17):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) & (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//ori
			case(18):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) | (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//xori
			case(19):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) ^ (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//addi
			case(20):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) + (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//subi
			case(21):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) - (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//muli
			case(22):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) * (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//divi
			case(23):{
				if(gprInput1 && gprOut){
					if((immediate)!=0){
						*gprOut = (*gprInput1) /(immediate);
						programCounter +=  simdSpec->instLength;
						return true;
					}
					else{

						if(simdSpec->gprBitLength == 4)
							*gprOut = UINT_MAX;
						else
							*gprOut = ULONG_MAX;
						return false;
					}

				}
				break;
			}
			//modi
			case(24):{
				if(gprInput1 && gprOut){
					if((*gprInput2)!=0){
						*gprOut = (*gprInput1) % (immediate);
						programCounter +=  simdSpec->instLength;
						return true;
					}
					else{
						if(simdSpec->gprBitLength == 4)
							*gprOut = UINT_MAX;
						else
							*gprOut = ULONG_MAX;
						return false;
					}
				}
				break;
			}
			//shli
			case(25):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) << (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//shri
			case(26):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) >> (immediate);
					programCounter +=  simdSpec->instLength;
					return true;
				}
				break;
			}
			//jali
			case(27):{
				if(gprOut){
					*gprOut = programCounter + simdSpec->instLength;
					programCounter +=  (immediate + simdSpec->instLength);
					return true;
				}
				break;
			}
			//jalr
			case(28):{
				if(gprInput1 && gprOut){
					T prevProgramCounter = programCounter;
					*gprOut = programCounter + simdSpec->instLength;
					programCounter =  (*gprInput1);
					if(programCounter == prevProgramCounter){
						return false;
					}
					return true;
				}
				break;
			}
			//jmpi
			case(29):{
				T prevProgramCounter = programCounter;
				programCounter +=  (immediate + simdSpec->instLength);
				if(programCounter == prevProgramCounter){
					return false;
				}
				return true;
				break;
			}

			//jmpr
			case(30):{
				if(gprInput1){
					T prevProgramCounter = programCounter;
					programCounter =  (*gprInput1);
					if(programCounter == prevProgramCounter){
						return false;
					}
					return true;
				}
				break;
			}
			//clone
			case(31):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//jalis
			case(32):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//jalrs
			case(33):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//jmprt
			case(34):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//ld
			case(35):{
				if(gprOut && gprInput1){
					T tempAddr = (*gprInput1) + (immediate);


					if(memoryMap->memoryBuff.find((uint64_t)tempAddr) == memoryMap->memoryBuff.end()){
						memoryMap->memoryBuff[(uint64_t)tempAddr] =  0;
					}

					T temp_data = (T)memoryMap->memoryBuff[(uint64_t)tempAddr];
					*gprOut = temp_data;

					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//st
			case(36):{
				if(gprInput1 && gprInput2){
					T temp_addr = (*gprInput2) + (immediate);

					if(temp_addr==simdSpec->writeAddr){
						outputMemory.push_back((char)(*gprInput1));
					}
					else{
						memoryMap->memoryBuff[(uint64_t)temp_addr] = *(gprInput1);
					}
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//ldi
			case(37):{
				if(gprOut){
					*gprOut = immediate;
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//rtop
			case(38):{
				if(gprInput1&&pregOut){
					if(*gprInput1){
						*pregOut = 1;
					}
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//andp
			case(39):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) & (*pregIn2);
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//orp
			case(40):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) | (*pregIn2);
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//xorp
			case(41):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) ^ (*pregIn2);
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//notp
			case(42):{
				if(pregIn1&&pregOut){
					*pregOut = ~(*pregIn1);
					programCounter += simdSpec->instLength;
					return true;
				}

				break;
			}
			//isneg
			case(43):{
				if(gprInput1&&pregOut){
					if(((*gprInput1 >> ((sizeof(T)*CHAR_BIT)-1))) == 1){
						*pregOut = 1;
					}
                    programCounter += simdSpec->instLength;
                    return true;
				}
				break;
			}
			//iszero
			case(44):{
				if(gprInput1&&pregOut){
					if((*gprInput1) == 0){
						*pregOut = 1;
					}
                    programCounter += simdSpec->instLength;
                    return true;
				}
				break;
			}
			//halt
			case(45):{
				//Baby I am done
				return false;
				break;
			}
			//trap
			case(46):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//jmpru
			case(47):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//skep
			case(48):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//reti
			case(49):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//tlbrm
			case(50):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//itof
			case(51):{
				if(gprOut&&gprInput1){
					*gprOut = *gprInput1;
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//ftoi
			case(52):{
				if(gprOut&&gprInput1){
					*gprOut = *gprInput1;
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//fadd
			case(53):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = *gprInput1 + *gprInput2;
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//fsub
			case(54):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = *gprInput1 - *gprInput2;
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//fmul
			case(55):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) * (*gprInput2);
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//fdiv
			case(56):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) / (*gprInput2);
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//fneg
			case(57):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) / (*gprInput2);
				}
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//wspawn
			case(58):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//split
			case(59):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//join
			case(60):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			//bar
			case(61):{
				programCounter += simdSpec->instLength;
				return true;
				break;
			}
			default:{
				return false;
				break;
			}

		}

		return false;

}


template class SimdCore<unsigned int>;
template class SimdCore<unsigned long long>;

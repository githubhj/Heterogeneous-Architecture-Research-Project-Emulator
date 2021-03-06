//============================================================================
// Name        : SimdCore.cpp
// Author      : Harshit Jain
// Version     : 1.0
// Copyright   : It's protected under Beerware
// GT ID       : 903024992
// Class       : ECE 8823
// Assignment  : 3
// Description : SimdCore Class in C++, Ansi-style
//============================================================================

#include "SimdCore.h"
#include "Warp.h"

/*
 * SimdCore constructor
 * params:
 * 1. ArchSpec_t*, pointer to arch specification as provided
 * 2. MemoryMap*: MemoryMap pointer to acces instructions
 * */
template<typename T>
SimdCore<T>::SimdCore(ArchSpec_t* simdSpec_i, MemoryMap* memoryMap_i){
    
    //Assign memory map and Arch specification
	simdSpec 		= simdSpec_i;
	memoryMap 		= memoryMap_i;

	//Initialize internal debug_counter
    debug_counter = 0;

    //Instantiaze Warp Queue as per Warp size
    for (uint64_t warpID =0 ; warpID < simdSpec->warpSize; warpID++) {
        warpQueue.push_back(new Warp<T>(simdSpec,warpID,false));
    }

    //Instantiate Warp Mask
    warpExecutionFlag = new bool[simdSpec->warpSize];
    
    //Initialize warp Execution Flag
    warpExecutionFlag[0] = true;
    for (uint64_t count = 1; count < simdSpec->warpSize; count++) {
        warpExecutionFlag[count] = false;
        warpQueue[count]->debugFlag = true;
    }
    warpQueue[0]->debugFlag = true;
    
    //Local swtich to turn off warp Execution
    warpQueue[0]->isExecuting = true;
    
    //Initiate predSign
    predSign = new bool[simdSpec->simdLaneSize];
    for (uint64_t count =0; count < simdSpec->simdLaneSize; count++) {
        predSign[count] = false;
    }
};


/*
 * SIMD fetch routine.
 * params: Takes a uint64_t or uint32_t programcounter to fetch from binary file
 * return: Instruction of uint64_t or uint32_t type.
 * */
template<typename T>
T SimdCore<T>::fetch(T programCounter){

	//return instruction
    T instruction =0;
    
    //Fetch and shift one byte at a time
    for (uint64_t count = 0; count <simdSpec->instLength; count++) {
        uint64_t tempAddr = programCounter + simdSpec->instLength -1 - count;
        instruction = (instruction << CHAR_BIT) | (uint64_t)memoryMap->memoryBuff[tempAddr];
    }
    
    //return instruction
    return instruction;
}


/*
 * SIMD fetch routine.
 * params: Takes a uint64_t or uint32_t programcounter to fetch from binary file.
 * A debug flag to print on console output for debug
 * return: Instruction of uint64_t or uint32_t type.
 * */
template<typename T>
T SimdCore<T>::fetch(T programCounter, bool debug){
	//Call non-debug fetch routine
    T instruction = fetch(programCounter);
    if (debug) {
        std::cerr << "\n------------------------------" << "\n";
        std::cerr << std::hex << "| PC | " << programCounter << " | Inst | " << instruction << " |\n";
        std::cerr << "------------------------------" << "\n";
    }
    return instruction;
}


/*
 * SIMD resetOperands routine.
 * params: None.
 * reset decoded operands value to -1
 * return: None.
 * */
template<typename T>
void SimdCore<T>::resetOperands(){
	//reset src 0
    gprSrc[0]       = -1;

    //reset src 1
    gprSrc[1]       = -1;

    //reset dst 0
    gprDest[0]      = -1;

    //reset predicate reg
    predReg         = -1;

    //reset predicate bit
    predicateBit    = false;

    //reset preg src 0
    pregSrc[0]      = -1;

    //reset preg src 1
    pregSrc[1]      = -1;

    //reset preg dst 0
    pregDest[0]     = -1;

    //reset immediate
    immediate       = -1;
}


/*
 * SIMD decode routine.
 * params: A fetched instruction of type uint64_t or uint32_t.
 * returns: opcode value, and sets the desired operands in SIMD class, unwanted operands are reset to -1.
 * */
template<typename T>
uint64_t SimdCore<T>::decode(T instruction){
    
	//Reset all operands
    resetOperands();

    //Return opcode Value
    uint64_t opcodeValue = 0;

    //Opcode to argument type
    ArgumentEnum_t	opcodeArg;
    
    //Get predicate Bit
    predicateBit = (instruction >> ((simdSpec->predicate.position)-1));
    
    
    //Get Opcode
    uint32_t opcodeBitsBefore 	= sizeof(T)*CHAR_BIT-simdSpec->opcode.position;
    uint32_t opcodeBitsAfter 	= simdSpec->opcode.position - simdSpec->opcode.length;
    opcodeValue = ((instruction << opcodeBitsBefore) >> opcodeBitsBefore) >> opcodeBitsAfter;
    
    //Get Arguments
    opcodeArg = simdSpec->opcodeToArgument[opcodeValue];
    
    //Get opcode with args
    uint32_t regShiftLeftVal = sizeof(T)*CHAR_BIT - (simdSpec->opcode.position - simdSpec->opcode.length);
    T opcodeWithArgs = (instruction << regShiftLeftVal) >> regShiftLeftVal;
    
    //Init shift vals
    uint32_t rightShiftVal1	=0;
    uint32_t rightShiftVal2	=0;
    uint32_t rightShiftVal3	=0;
    uint32_t leftShiftVal1  =0;
    uint32_t leftShiftVal2  =0;
    
    //Decode as per argument class
    switch(opcodeArg){

    	//AC_NONE: Nothing to decode
        case(ArgumentEnum::AC_NONE):{
            break;
        }

        //AC_2REG: Get src 0  and dest 0
        case(ArgumentEnum::AC_2REG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            break;
        }

        //AC_2IMM: Get immediate and dest 0
        case(ArgumentEnum::AC_2IMM):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            immediate		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1);
            
            //If immediate is negative
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

        //AC_3REG: Get src0, src1 and dest0
        case(ArgumentEnum::AC_3REG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            rightShiftVal3	= rightShiftVal2 - simdSpec->gprBitLength;
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            gprSrc[1] 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
            
            break;
        }

        //AC_3PREG: Get Preg Src0, Preg Src1 and Preg dest0
        case (ArgumentEnum::AC_3PREG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->pregBitLength;
            rightShiftVal3	= rightShiftVal2 - simdSpec->pregBitLength;
            pregDest[0] 	= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
            pregSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->pregBitLength;
            pregSrc[1] 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
            break;
        }

        //Ac_3IMM: Get scr0, dest0 and immediate value.
        case(ArgumentEnum::AC_3IMM):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprDest[0] 	= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            immediate 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);
            
            //If immediate is negative
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

        //AC_3REGSRC: Get src 0 , src 1 and dest 0.
        case(ArgumentEnum::AC_3REGSRC):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            rightShiftVal3	= rightShiftVal2 - simdSpec->gprBitLength;
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            gprSrc[1] 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
            break;
        }

        //AC_1IMM: Get immediate value
        case(ArgumentEnum::AC_1IMM):{
            immediate = opcodeWithArgs ;
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length);

            //If immediate is negative
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

        //AC_1REG: Get src0.
        case(ArgumentEnum::AC_1REG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            gprSrc[0] 		= opcodeWithArgs >> rightShiftVal1;
            break;
        }

        //AC_3IMMSRC: Get scr0, dest 0 and immediate value.
        case(ArgumentEnum::AC_3IMMSRC):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprSrc[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[1]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            immediate 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);

            //If immediate is negative.
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

        //AC_PREG_REG: Get preg dest 0, gpr src 0.
        case(ArgumentEnum::AC_PREG_REG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            pregDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
            gprSrc[0]			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
        
            break;
        }

        //AC_2PREG: Get preg dest 0 and preg src 0.
        case(ArgumentEnum::AC_2PREG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->pregBitLength;
            pregDest[0]		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
            pregSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            
            break;
        }
        default:{
            return false;
            break;
        }
    }
    
    
    //Get Predicate register
    predReg =  ((instruction << (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (simdSpec->pregRegField.position - simdSpec->pregRegField.length);
    
    //Return opcode Value
    return opcodeValue;
}

/*
 * SIMD decode routine with debug flag.
 * params	: same as decode with debug flag to print stats.
 * returns	: returns decoded opcode value.
 * */
template<typename T>
uint64_t SimdCore<T>::decode(T instruction, bool debug){

	//Call normal decode.
    uint64_t oVal = decode(instruction);
    
    //Print stats
    if (debug) {
        std::cerr << "\n-------------------------\n";
        std::cerr << "| PredBit | PReg | Opcode |\n";
        std::cerr << "---------------------------\n";
        std::cerr << std::hex << "| " << predicateBit << " | " << predReg << " | " << oVal << " |\n";
        std::cerr << "---------------------------\n";
    }
    
    //return oval
    return oVal;
}

/*
 * SIMD execute routine.
 * params:	1. Decoded opcode value.
 * 			2. Warp ID executing opcode.
 * 			3. LaneID of given warp executing instruction.
 * returns:	False if alt or loop to itself executed else, True.
 * */
template<typename T>
bool SimdCore<T>::execute(uint64_t opCodeValue, uint64_t warpID, uint64_t laneID){
    
	//Check predicate bit
    if(predicateBit){
    	//If set check Predicate register
        if(warpQueue[warpID]->pregFile[laneID][predReg]==false){
        	//Don't execute this instruction, just increment pc and return.
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
        }
    }
    
    //Execute instruction
    switch(opCodeValue){
            //NOP
        case(0):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //di
        case(1):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //ei
        case(2):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //tlbadd
        case(3):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
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
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = -warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //not
        case(6):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = ~warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //and
        case(7):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] & warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //or
        case(8):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] | warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //xor
        case(9):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] ^ warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //add
        case(10):{
            if(  (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] + warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //sub
        case(11):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] - warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //mul
        case(12):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] * warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //div
        case(13):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                if(warpQueue[warpID]->gprFile[laneID][gprSrc[1]]!=0){
                    warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] / warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
                
            }
            break;
        }
            //mod
        case(14):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                if(warpQueue[warpID]->gprFile[laneID][gprSrc[1]]!=0){
                    warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] % warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
            }
            break;
        }
            //shl
        case(15):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] << warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //shr
        case(16):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] >> warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //andi
        case(17):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] & immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //ori
        case(18):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] | immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //xori
        case(19):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] ^ immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //addi
        case(20):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] + immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //subi
        case(21):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] - immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //muli
        case(22):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] * immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //divi
        case(23):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                if((immediate)!=0){
                    warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] / immediate;
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    
                    if(simdSpec->gprBitLength == 4)
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
                
            }
            break;
        }
            //modi
        case(24):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                if((immediate)!=0){
                    warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] % immediate;
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        warpQueue[warpID]->gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
            }
            break;
        }
            //shli
        case(25):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] << immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //shri
        case(26):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] >> immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //jali
        case(27):{
            if(gprDest[0]!=-1){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->programCounter + simdSpec->instLength;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + (immediate + simdSpec->instLength);
                return true;
            }
            break;
        }
            //jalr
        case(28):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->programCounter + simdSpec->instLength;
                
                warpQueue[warpID]->nextProgramCounter =  warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                
                if(warpQueue[warpID]->nextProgramCounter == warpQueue[warpID]->programCounter){
                    return false;
                }
                return true;
            }
            break;
        }
            //jmpi
        case(29):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + (immediate + simdSpec->instLength);
            
            if(warpQueue[warpID]->programCounter == warpQueue[warpID]->nextProgramCounter){
                return false;
            }
            return true;
            break;
        }
            
            //jmpr
        case(30):{

            //get Lowest Active laneID, this laneID will execute this intruction no other lane ID.
            uint64_t lowestAvtiveLaneID = 0;
            for (uint64_t count = 0; count < simdSpec->simdLaneSize; count++) {
                if (warpQueue[warpID]->currMask[count]) {
                    lowestAvtiveLaneID =count;
                    break;
                }
            }
            
            //Execute this instruction
            if(gprSrc[0]!=-1){
                
                if (laneID == lowestAvtiveLaneID) {
                    warpQueue[warpID]->nextProgramCounter =  warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                }
                
                if(warpQueue[warpID]->programCounter == warpQueue[warpID]->nextProgramCounter){
                    return false;
                }
                return true;
            }
            break;
        }
            //clone
        case(31):{
        	//Increment PC
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            if(gprSrc[0]!=-1){

            	//Copy GPR
                for(int i=0 ; i< simdSpec->gprNum ; i++){
                    warpQueue[warpID]->gprFile[warpQueue[warpID]->gprFile[laneID][gprSrc[0]]][i] = warpQueue[warpID]->gprFile[laneID][i];
                }

                //Copy PREG
                for(int i=0 ; i< simdSpec->pregNum ; i++){
                    warpQueue[warpID]->pregFile[warpQueue[warpID]->gprFile[laneID][gprSrc[0]]][i] = warpQueue[warpID]->pregFile[laneID][i];
                }
                
                return true;
            }
            
            break;
        }
            //jalis
        case(32):{
            
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->programCounter + simdSpec->instLength;
                
                //Change activity mask
                for (uint64_t i =0 ; i < warpQueue[warpID]->gprFile[laneID][gprSrc[0]]; i++) {
                    warpQueue[warpID]->nextActMask[i] = true;
                }

                //Change activity mask
                for (uint64_t i = warpQueue[warpID]->gprFile[laneID][gprSrc[0]]; i < simdSpec->simdLaneSize; i++) {
                    warpQueue[warpID]->nextActMask[i] = false;
                }
                return true;
            }
            break;
        }
            //jalrs
        case(33):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1)){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->programCounter + simdSpec->instLength;
                
                //Get next PC
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                
                //Change activity mask
                for (uint64_t i =0 ; i < warpQueue[warpID]->gprFile[laneID][gprSrc[0]]; i++) {
                    warpQueue[warpID]->nextActMask[i] = true;
                }
                for (uint64_t i = warpQueue[warpID]->gprFile[laneID][gprSrc[0]]; i < simdSpec->simdLaneSize; i++) {
                    warpQueue[warpID]->nextActMask[i] = false;
                }
                return true;
            }
            break;
        }
            //jmprt
        case(34):{
            
            //get Lowest Active laneID
            uint64_t lowestAvtiveLaneID = 0;
            for (uint64_t count = 0; count < simdSpec->simdLaneSize; count++) {
                if (warpQueue[warpID]->currMask[count]) {
                    lowestAvtiveLaneID =count;
                    break;
                }
            }
            
            //Change only from lowest lane ID
            if( (gprSrc[0]!=-1) ){
                
                if (laneID == lowestAvtiveLaneID) {
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                    
                    warpQueue[warpID]->nextActMask[laneID] = true;
                    for (uint64_t i = 0; i < simdSpec->simdLaneSize; i++) {
                        if(i!=laneID)
                            warpQueue[warpID]->nextActMask[i] = false;
                    }
                }
        
                return true;
            }
            break;
        }
            //ld
        case(35):{
            if((gprSrc[0]!=-1) && (gprDest[0]!=-1)){
                T actualAddr = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] + (immediate);
                T temp_data =0;

                //Load value from memory
                for (uint64_t count =0; count < simdSpec->instLength; count++) {
                    uint64_t tempAddr = actualAddr + simdSpec->instLength -1 -count;
                    
                    if(memoryMap->memoryBuff.find((uint64_t)tempAddr) == memoryMap->memoryBuff.end()){
                        memoryMap->memoryBuff[(uint64_t)tempAddr] =  0;
                    }
                    temp_data = temp_data << CHAR_BIT | (uint64_t)memoryMap->memoryBuff[tempAddr];
                    
                }
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = temp_data;

                //increment npc
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //st
        case(36):{
            if((gprSrc[0]!=-1) && (gprSrc[1]!=-1)){
                T actualAddr = warpQueue[warpID]->gprFile[laneID][gprSrc[1]] + (immediate);
                
                //If addr is write addr and laneID is 0, write to output memory
                if(actualAddr==simdSpec->writeAddr && laneID == 0){
                    outputMemory.push_back((char)(warpQueue[warpID]->gprFile[laneID][gprSrc[0]]));
                }
                //Else write to memory map
                else{
                    T tempAddr = actualAddr;
                    for (uint64_t count =0; count < simdSpec->instLength; count++) {
                        memoryMap->memoryBuff[(uint64_t)tempAddr] = (uint8_t)(warpQueue[warpID]->gprFile[laneID][gprSrc[0]] >> (CHAR_BIT*count));
                        tempAddr++;
                    }
                    
                }
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //ldi
        case(37):{
            if(gprDest[0]!=-1){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = immediate;
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //rtop
        case(38):{
            if(gprSrc[0]!=-1 && (pregDest[0]!=-1)){
                if(warpQueue[warpID]->gprFile[laneID][gprSrc[0]]){
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]] = false;
                }
                warpQueue[warpID]->nextProgramCounter  = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //andp
        case(39):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                warpQueue[warpID]->pregFile[laneID][pregDest[0]] = warpQueue[warpID]->pregFile[laneID][pregSrc[0]] & warpQueue[warpID]->pregFile[laneID][pregSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //orp
        case(40):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                warpQueue[warpID]->pregFile[laneID][pregDest[0]] = warpQueue[warpID]->pregFile[laneID][pregSrc[0]] | warpQueue[warpID]->pregFile[laneID][pregSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //xorp
        case(41):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                warpQueue[warpID]->pregFile[laneID][pregDest[0]] = warpQueue[warpID]->pregFile[laneID][pregSrc[0]] ^ warpQueue[warpID]->pregFile[laneID][pregSrc[1]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //notp
        case(42):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) ) {
                warpQueue[warpID]->pregFile[laneID][pregDest[0]] = !warpQueue[warpID]->pregFile[laneID][pregSrc[0]];
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //isneg
        case(43):{
            if( (gprSrc[0]!=-1) && (pregDest[0]!=-1) ){
                if(((warpQueue[warpID]->gprFile[laneID][gprSrc[0]] >> ((sizeof(T)*CHAR_BIT)-1))) == 1){
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]] = false;
                }
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //iszero
        case(44):{
            if( (gprSrc[0]!=-1) && (pregDest[0]!=-1) ){
                if((warpQueue[warpID]->gprFile[laneID][gprSrc[0]]) == 0){
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    warpQueue[warpID]->pregFile[laneID][pregDest[0]]  = false;
                }
                warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
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
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //jmpru
        case(47):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //skep
        case(48):{
            warpQueue[warpID]->nextProgramCounter  = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //reti
        case(49):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //tlbrm
        case(50):{
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //itof
        case(51):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //ftoi
        case(52):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fadd
        case(53):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] + warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fsub
        case(54):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] - warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fmul
        case(55):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] * warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fdiv
        case(56):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] / warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fneg
        case(57):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                warpQueue[warpID]->gprFile[laneID][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[0]] / warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
            }
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //wspawn
        case(58):{
            
            //get Lowest Active laneID
            uint64_t lowestAvtiveLaneID = 0;
            for (uint64_t count = 0; count < simdSpec->simdLaneSize; count++) {
                if (warpQueue[warpID]->currMask[count]) {
                    lowestAvtiveLaneID =count;
                    break;
                }
            }
            
            //Execute only from lowest laneID
            if(laneID == lowestAvtiveLaneID){
                if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                	//For each warp
                    for (uint64_t warpC =0 ; warpC < simdSpec->warpSize; warpC++) {
                    	//Get lowest inactive warp
                        if(!warpExecutionFlag[warpC]){
                        	//Make this warp active
                            warpExecutionFlag[warpC] =true;

                            //Change its npc, pc and dest reg
                            warpQueue[warpC]->programCounter =  warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                            warpQueue[warpC]->nextProgramCounter =  warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                            warpQueue[warpC]->gprFile[0][gprDest[0]] = warpQueue[warpID]->gprFile[laneID][gprSrc[1]];
                            
                            //Make 0th lane active
                            warpQueue[warpC]->currMask[0] = true;
                            warpQueue[warpC]->nextActMask[0] = true;
                            break;
                        }
                    }
                }
            }
            
            //Inc PC
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
            
            return true;
            break;
        }
            //split
        case(59):{
            
            //Main execution to be done by start routine
            warpQueue[warpID]->splitSign = true;
            
            return true;
            break;
        }
            //join
        case(60):{
            
        	//Main execution to be done by start routine
            warpQueue[warpID]->joinSign = true;
            
            return true;
            break;
        }
            //bar
        case(61):{

        	//Increment PC
            warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;

            //get Lowest Active laneID
            uint64_t lowestAvtiveLaneID = 0;
            for (uint64_t count = 0; count < simdSpec->simdLaneSize; count++) {
                if (warpQueue[warpID]->currMask[count]) {
                    lowestAvtiveLaneID =count;
                    break;
                }
            }
            
            //Execute from lowest laneID
            if (laneID == lowestAvtiveLaneID) {

                if ( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ) {

                	//Get ID and warp num.
                    T id = warpQueue[warpID]->gprFile[laneID][gprDest[0]];
                    T n = warpQueue[warpID]->gprFile[laneID][gprSrc[0]];
                    
                    //Add warp to barrier hash.
                    barrier[id].insert(warpQueue[warpID]);

                    //Make all lanes of warp in active
                    for (uint64_t laneC=0; laneC < simdSpec->simdLaneSize; laneC++) {
                        warpQueue[warpID]->shadowMask[laneC] = warpQueue[warpID]->currMask[laneC];
                        warpQueue[warpID]->nextActMask[laneC] = false;
                    }

                    //Flag this warp as in barrier
                    warpQueue[warpID]->inBarrier = true;
                    
                    //If barrier is reached by last warp
                    if (barrier[id].size()==n) {

                    	//Start unravelling other warps
                        typename std::set<Warp<T>* >::iterator it;
                        for (it=barrier[id].begin(); it!=barrier[id].end(); it++) {

                        	//Make current mask with shadow mask
                            for (uint64_t laneC=0; laneC<simdSpec->simdLaneSize; laneC++) {
                                (*it)->currMask[laneC] = (*it)->shadowMask[laneC];
                                (*it)->nextActMask[laneC] = (*it)->shadowMask[laneC];
                            }

                            //Make in barrier false
                            (*it)->inBarrier = false;
                        }

                        //Erase this barrier ID
                        barrier.erase(id);
                        
                    }
                    
                }
            }
            
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

/*
 * Debug mode execution.
 * params	: Same as normal execute but with debug.
 * returns	: Same as normal execution.
 * A routine of actually no use.
 * */
template<typename T>
bool SimdCore<T>::execute(uint64_t opCodeValue,  uint64_t warpID, uint64_t laneID, bool debug){
    bool retVal = execute(opCodeValue, warpID, laneID);
    if(debug){
//        std::cerr << "\n-------------------------\n";
//        std::cerr << "Executing\n";
//        std::cerr << "---------------------------\n";
    }
    return retVal;
}

/*
 * SIMD start routine.
 * THIS ROUTINE KICKS ON THE "SHIT"
 * params	: Just a lil bool.
 * returns	: Nothing \m/.
 * */
template<typename T>
void SimdCore<T>::start(bool debug){
    
    //Local control variables
    //Checks if all warps are exhausted or not
	bool executeNext = true;
    bool executeOnce = false;
    T instruction;
    uint64_t opCodeVal =0;
    
    //Execute Instruction
    while (executeNext) {
    	//Toggle Switch
    	executeNext = false;

    	//For each Warp
        for (uint64_t warpID=0; warpID < simdSpec->warpSize; warpID++) {
            
            //Debug
            warpQueue[warpID]->debugFlag = debug;
            
        	//If warp is active
            if(warpExecutionFlag[warpID]){
            	//Toggle Switch
            	executeNext = true;
                
                //PC = NPC
                warpQueue[warpID]->programCounter = warpQueue[warpID]->nextProgramCounter;
                
                //Current Mask = Next Active MAsk
                for (uint64_t maskID =0; maskID < simdSpec->simdLaneSize; maskID++) {
                    warpQueue[warpID]->currMask[maskID] = warpQueue[warpID]->nextActMask[maskID];
                }
                
                //Fetch
                instruction = fetch(warpQueue[warpID]->programCounter, debug);
                
                //Decode
                opCodeVal = decode(instruction, debug);
                
                executeOnce = false;
                
                //Execute Per Lane
                for (uint64_t laneID = 0; laneID < simdSpec->simdLaneSize; laneID++) {
                    if (warpQueue[warpID]->currMask[laneID]) {
                        executeOnce = true;
                        warpExecutionFlag[warpID] = execute(opCodeVal, warpID, laneID, debug);
                    }
                }
                
                //If not executed Once
                if (!executeOnce && !warpQueue[warpID]->inBarrier) {
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    warpExecutionFlag[warpID] = true;
                }
                
                if ((!warpQueue[warpID]->inBarrier) && (warpQueue[warpID]->splitSign || opCodeVal == 0x3b) ) {
                    //PC
                    warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    
                    warpExecutionFlag[warpID] = true;
                    
                    //Make split sign false
                    warpQueue[warpID]->splitSign = false;
                    
                    //Temp stack elem
                    reconvStackElem<T> elem1;
                    
                    //Push arbit PC and curr mask
                    elem1.nextPC = -1;
                    elem1.activityMask = new bool[simdSpec->simdLaneSize];
                    for(uint64_t count = 0 ; count < simdSpec->simdLaneSize; count++){
                        elem1.activityMask[count] = warpQueue[warpID]->currMask[count];
                    }
                    warpQueue[warpID]->reconvStack.push_back(elem1);
                    
                    //Temp stack elem
                    reconvStackElem<T> elem2;
                    
                    //Push Next PC and ~next mask
                    elem2.nextPC = warpQueue[warpID]->nextProgramCounter;
                    elem2.activityMask = new bool[simdSpec->simdLaneSize];
                    for (uint64_t predC=0; predC < simdSpec->simdLaneSize; predC++) {
                        predSign[predC] = (!predicateBit || warpQueue[warpID]->pregFile[predC][predReg]) && warpQueue[warpID]->currMask[predC];
                    }
                    
                    for (uint64_t count =0 ; count < simdSpec->simdLaneSize; count++) {
                        elem2.activityMask[count] = (warpQueue[warpID]->currMask[count] && !predSign[count]);
                    }
                    
                    warpQueue[warpID]->reconvStack.push_back(elem2);
                    
                    //Change next mask
                    for (uint64_t count =0 ; count < simdSpec->simdLaneSize; count++) {
                        warpQueue[warpID]->nextActMask[count] = warpQueue[warpID]->currMask[count] && predSign[count];
                    }
                    
                    //Set predicate signature to 0
                    for (uint64_t pC=0; pC < simdSpec->simdLaneSize; pC++) {
                        predSign[pC] = false;
                    }

                }
                
                //check if join was executed last
                if((!warpQueue[warpID]->inBarrier) && (warpQueue[warpID]->joinSign || (opCodeVal == 0x3c))){
                    warpQueue[warpID]->joinSign = false;
                    
                    executeNext = true;
                    
                    if(warpQueue[warpID]->reconvStack.back().nextPC!=-1){
                        warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->reconvStack.back().nextPC;
                    }
                    else{
                        warpQueue[warpID]->nextProgramCounter = warpQueue[warpID]->programCounter + simdSpec->instLength;
                    }
                    
                    for(uint64_t count =0; count<simdSpec->simdLaneSize; count++){
                        warpQueue[warpID]->nextActMask[count]  = warpQueue[warpID]->reconvStack.back().activityMask[count];
                    }
                    warpQueue[warpID]->reconvStack.pop_back();
                }
                
            }
            
            //Steps
            warpQueue[warpID]->debug_counter++;
            
            //print stats
            warpQueue[warpID]->printStats(debug);
            instruction = 0;
        }
    }
}


//Memory output for a SIMD lane
template <typename T>
std::string SimdCore<T>::getOutputData(){
    std::string myStr;
    for (uint64_t i=0; i<outputMemory.size(); i++) {
        myStr += outputMemory[i];
    }
 
    return myStr;
}

//Clear Output Memory counter
template <typename T>
void SimdCore<T>::clearOutPutMemory(){
    outputMemory.resize(0);
    outputMemory.shrink_to_fit();
}


//Reset the object
template <typename T>
void SimdCore<T>::reset(){
    for (uint64_t count =0 ; count < simdSpec->warpSize; count++) {
        warpQueue[count]->reset();
    }
    clearOutPutMemory();
    resetOperands();
}




//Copy constructor
template <typename T>
SimdCore<T>::SimdCore(const SimdCore& other){
    simdSpec 		= other.simdSpec;
    memoryMap 		= other.memoryMap;
    debug_counter = other.debug_counter;
}

//Destructor
template <typename T>
SimdCore<T>::~SimdCore(){
    delete[] warpExecutionFlag;
    for (uint64_t count =0; count < simdSpec->warpSize; count++) {
        delete warpQueue[count];
    }
}

template class SimdCore<uint32_t>;
template class SimdCore<uint64_t>;


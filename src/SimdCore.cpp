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

template<typename T>
SimdCore<T>::SimdCore(ArchSpec_t* simdSpec_i, MemoryMap* memoryMap_i){
    
    //Assign memory map and Arch specification
	simdSpec 		= simdSpec_i;
	memoryMap 		= memoryMap_i;
    
    //Init gprFile
    for (uint64_t i=0; i<simdSpec->simdLaneSize; i++) {
        gprFile.push_back(new T[simdSpec->gprNum]);
        for (int j=0; j<simdSpec->gprNum; j++) {
            gprFile[i][j] = DEFAULT_GPRVAL;
        }
    }
    
    //Init PregFile
    for (uint64_t i=0; i<simdSpec->simdLaneSize; i++) {
        pregFile.push_back(new bool[simdSpec->pregNum]);
        for (int j=0; j<simdSpec->pregNum; j++) {
            pregFile[i][j] = DEFAULT_PREGVAL;
        }
        
    }
    
    //Init Mask
    currMask = new bool[simdSpec->simdLaneSize];
    nextActMask = new bool[simdSpec->simdLaneSize];
    currMask[0] = true;
    nextActMask[0] = true;
    for (uint64_t i=1; i<simdSpec->simdLaneSize; i++) {
        currMask[i] = false;
        nextActMask[i] = false;
    }
    
    //Init Predicate Signature for split and join
    predSign = new bool[simdSpec->simdLaneSize];
    for (uint64_t pC=0; pC<simdSpec->simdLaneSize; pC++) {
        predSign[pC] = false;
    }
    
    //Init split and join signature
    splitSign = false;
    joinSign  = false;
    
    //Init Program Counter
	programCounter	= 0;
    debug_counter = 0;
};


//Simd fetch
template<typename T>
T SimdCore<T>::fetch(){
    T instruction =0;
    
    for (uint64_t count = 0; count <simdSpec->instLength; count++) {
        uint64_t tempAddr = programCounter + simdSpec->instLength -1 - count;
        instruction = (instruction << CHAR_BIT) | (uint64_t)memoryMap->memoryBuff[tempAddr];
    }
    
    return instruction;
}


//Simd fetch with debug
template<typename T>
T SimdCore<T>::fetch(bool debug){
    T instruction = fetch();
    if (debug) {
        std::cerr << "\n-----------------------------------" << "\n";
        std::cerr << std::hex << "| PC | " << programCounter << " | Inst | " << instruction << " |\n";
        std::cerr << "-----------------------------------" << "\n";
    }
    return instruction;
}


template<typename T>
void SimdCore<T>::resetOperands(){
    gprSrc[0]       = -1;
    gprSrc[1]       = -1;
    gprDest[0]      = -1;
    predReg         = -1;
    predicateBit    = false;
    pregSrc[0]      = -1;
    pregSrc[1]      = -1;
    pregDest[0]     = -1;
    immediate       = -1;
}


//Simd decode
template<typename T>
uint64_t SimdCore<T>::decode(T instruction){
    
    resetOperands();
    uint64_t opcodeValue =0;
    ArgumentEnum_t	opcodeArg;
    
    //Get predicate Bit
    predicateBit = (instruction >> ((simdSpec->predicate.position)-1));
    
    //Normal Execution
    
    //Get Opcode
    uint32_t opcodeBitsBefore 	= sizeof(T)*CHAR_BIT-simdSpec->opcode.position;
    uint32_t opcodeBitsAfter 	= simdSpec->opcode.position - simdSpec->opcode.length;
    opcodeValue = ((instruction << opcodeBitsBefore) >> opcodeBitsBefore) >> opcodeBitsAfter;
    
    //Get Arguments
    opcodeArg = simdSpec->opcodeToArgument[opcodeValue];
    
    //Get opcode with args
    uint32_t regShiftLeftVal = sizeof(T)*CHAR_BIT - (simdSpec->opcode.position - simdSpec->opcode.length);
    T opcodeWithArgs = (instruction << regShiftLeftVal) >> regShiftLeftVal;
    
    uint32_t rightShiftVal1	=0;
    uint32_t rightShiftVal2	=0;
    uint32_t rightShiftVal3	=0;
    uint32_t leftShiftVal1  =0;
    uint32_t leftShiftVal2  =0;
    
    switch(opcodeArg){
            
        case(ArgumentEnum::AC_NONE):{
            break;
        }
        case(ArgumentEnum::AC_2REG):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            break;
        }
        case(ArgumentEnum::AC_2IMM):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            immediate		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1);
            
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
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            gprSrc[1] 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
            
            break;
        }
            
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
            
        case(ArgumentEnum::AC_3IMM):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprDest[0] 	= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            immediate 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);
            
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
            gprDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[0]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            gprSrc[1] 		= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2) >> rightShiftVal3;
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
            gprSrc[0] 		= opcodeWithArgs >> rightShiftVal1;
            break;
        }
            
        case(ArgumentEnum::AC_3IMMSRC):{
            rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
            rightShiftVal2 	= rightShiftVal1 - simdSpec->gprBitLength;
            gprSrc[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
            gprSrc[1]		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
            leftShiftVal2  	= leftShiftVal1 + simdSpec->gprBitLength;
            immediate 			= ((opcodeWithArgs << leftShiftVal2) >> leftShiftVal2);
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
            pregDest[0] 		= opcodeWithArgs >> rightShiftVal1;
            leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
            gprSrc[0]			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;
        
            break;
        }
            
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
    
    
    //Predicated check
    predReg =  ((instruction << (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (simdSpec->pregRegField.position - simdSpec->pregRegField.length);
    
    return opcodeValue;
}

//Simd decode
template<typename T>
uint64_t SimdCore<T>::decode(T instruction, bool debug){
    uint64_t oVal = decode(instruction);
    
    if (debug) {
        std::cerr << "\n-------------------------\n";
        std::cerr << "| PredBit | PReg | Opcode |\n";
        std::cerr << "---------------------------\n";
        std::cerr << std::hex << "| " << predicateBit << " | " << predReg << " | " << oVal << " |\n";
        std::cerr << "---------------------------\n";
    }
    
    return oVal;
}

template<typename T>
bool SimdCore<T>::execute(uint64_t opCodeValue, uint64_t laneID){
    
    //get Lowest Active laneID
    uint64_t lowestAvtiveLaneID = 0;
    for (uint64_t count = 0; count < simdSpec->simdLaneSize; count++) {
        if (currMask[count]) {
            lowestAvtiveLaneID =count;
            break;
        }
    }
    
    if(predicateBit){
        if(pregFile[laneID][predReg]==false){
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
        }
    }
    
    switch(opCodeValue){
            //NOP
        case(0):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //di
        case(1):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //ei
        case(2):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //tlbadd
        case(3):{
            nextProgramCounter = programCounter + simdSpec->instLength;
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
                gprFile[laneID][gprDest[0]] = -gprFile[laneID][gprSrc[0]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //not
        case(6):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = ~gprFile[laneID][gprSrc[0]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //and
        case(7):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] & gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //or
        case(8):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] | gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //xor
        case(9):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] ^ gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //add
        case(10):{
            if(  (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] + gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //sub
        case(11):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] - gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //mul
        case(12):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] * gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //div
        case(13):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                if(gprFile[laneID][gprSrc[1]]!=0){
                    gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] / gprFile[laneID][gprSrc[1]];
                    nextProgramCounter = programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
                
            }
            break;
        }
            //mod
        case(14):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                if(gprFile[laneID][gprSrc[1]]!=0){
                    gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] % gprFile[laneID][gprSrc[1]];
                    nextProgramCounter = programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
            }
            break;
        }
            //shl
        case(15):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] << gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //shr
        case(16):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] >> gprFile[laneID][gprSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //andi
        case(17):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] & immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //ori
        case(18):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] | immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //xori
        case(19):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] ^ immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //addi
        case(20):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] + immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //subi
        case(21):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] - immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //muli
        case(22):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] * immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //divi
        case(23):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                if((immediate)!=0){
                    gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] / immediate;
                    nextProgramCounter = programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    
                    if(simdSpec->gprBitLength == 4)
                        gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
                
            }
            break;
        }
            //modi
        case(24):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                if((immediate)!=0){
                    gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] % immediate;
                    nextProgramCounter = programCounter + simdSpec->instLength;
                    return true;
                }
                else{
                    if(simdSpec->gprBitLength == 4)
                        gprFile[laneID][gprDest[0]] = UINT_MAX;
                    else
                        gprFile[laneID][gprDest[0]] = ULONG_MAX;
                    return false;
                }
            }
            break;
        }
            //shli
        case(25):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] << immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //shri
        case(26):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] >> immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //jali
        case(27):{
            if(gprDest[0]!=-1){
                gprFile[laneID][gprDest[0]] = programCounter + simdSpec->instLength;
                nextProgramCounter = programCounter + (immediate + simdSpec->instLength);
                return true;
            }
            break;
        }
            //jalr
        case(28):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = programCounter + simdSpec->instLength;
                
                nextProgramCounter =  gprFile[laneID][gprSrc[0]];
                
                if(nextProgramCounter == programCounter){
                    return false;
                }
                return true;
            }
            break;
        }
            //jmpi
        case(29):{
            nextProgramCounter = programCounter + (immediate + simdSpec->instLength);
            
            if(programCounter == nextProgramCounter){
                return false;
            }
            return true;
            break;
        }
            
            //jmpr
        case(30):{
            if(gprSrc[0]!=-1){
                
                if (laneID == lowestAvtiveLaneID) {
                    nextProgramCounter =  gprFile[laneID][gprSrc[0]];
                }
                
                if(programCounter == nextProgramCounter){
                    return false;
                }
                return true;
            }
            break;
        }
            //clone
        case(31):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            if(gprSrc[0]!=-1){
                for(int i=0 ; i< simdSpec->gprNum ; i++){
                    gprFile[gprFile[laneID][gprSrc[0]]][i] = gprFile[laneID][i];
                }
                for(int i=0 ; i< simdSpec->pregNum ; i++){
                    pregFile[gprFile[laneID][gprSrc[0]]][i] = pregFile[laneID][i];
                }
                
                return true;
            }
            
            break;
        }
            //jalis
        case(32):{
            
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = programCounter + simdSpec->instLength;
                
                for (uint64_t i =0 ; i < gprFile[laneID][gprSrc[0]]; i++) {
                    nextActMask[i] = true;
                }
                for (uint64_t i = gprFile[laneID][gprSrc[0]]; i < simdSpec->simdLaneSize; i++) {
                    nextActMask[i] = false;
                }
                return true;
            }
            break;
        }
            //jalrs
        case(33):{
            if( (gprSrc[0]!=-1) && (gprSrc[1]!=-1) && (gprDest[0]!=-1)){
                gprFile[laneID][gprDest[0]] = programCounter + simdSpec->instLength;
                
                nextProgramCounter = gprFile[laneID][gprSrc[1]];
                
                for (uint64_t i =0 ; i < gprFile[laneID][gprSrc[0]]; i++) {
                    nextActMask[i] = true;
                }
                for (uint64_t i = gprFile[laneID][gprSrc[0]]; i < simdSpec->simdLaneSize; i++) {
                    nextActMask[i] = false;
                }
                return true;
            }
            break;
        }
            //jmprt
        case(34):{
            if( (gprSrc[0]!=-1) ){
                
                if (laneID == lowestAvtiveLaneID) {
                    nextProgramCounter = gprFile[laneID][gprSrc[0]];
                    
                    nextActMask[laneID] = true;
                    for (uint64_t i = 0; i < simdSpec->simdLaneSize; i++) {
                        if(i!=laneID)
                            nextActMask[i] = false;
                    }
                }
        
                return true;
            }
            break;
        }
            //ld
        case(35):{
            if((gprSrc[0]!=-1) && (gprDest[0]!=-1)){
                T actualAddr = gprFile[laneID][gprSrc[0]] + (immediate);
                T temp_data =0;
                for (uint64_t count =0; count < simdSpec->instLength; count++) {
                    uint64_t tempAddr = actualAddr + simdSpec->instLength -1 -count;
                    
                    if(memoryMap->memoryBuff.find((uint64_t)tempAddr) == memoryMap->memoryBuff.end()){
                        memoryMap->memoryBuff[(uint64_t)tempAddr] =  0;
                    }
                    temp_data = temp_data << CHAR_BIT | (uint64_t)memoryMap->memoryBuff[tempAddr];
                    
                }
                gprFile[laneID][gprDest[0]] = temp_data;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //st
        case(36):{
            if((gprSrc[0]!=-1) && (gprSrc[1]!=-1)){
                T actualAddr = gprFile[laneID][gprSrc[1]] + (immediate);
                
                if(actualAddr==simdSpec->writeAddr && laneID == 0){
                    outputMemory.push_back((char)(gprFile[laneID][gprSrc[0]]));
                }
                else{
                    T tempAddr = actualAddr;
                    for (uint64_t count =0; count < simdSpec->instLength; count++) {
                        memoryMap->memoryBuff[(uint64_t)tempAddr] = (uint8_t)(gprFile[laneID][gprSrc[0]] >> (CHAR_BIT*count));
                        tempAddr++;
                    }
                    
                }
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //ldi
        case(37):{
            if(gprDest[0]!=-1){
                gprFile[laneID][gprDest[0]] = immediate;
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //rtop
        case(38):{
            if(gprSrc[0]!=-1 && (pregDest[0]!=-1)){
                if(gprFile[laneID][gprSrc[0]]){
                    pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    pregFile[laneID][pregDest[0]] = false;
                }
                nextProgramCounter  = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //andp
        case(39):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                pregFile[laneID][pregDest[0]] = pregFile[laneID][pregSrc[0]] & pregFile[laneID][pregSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //orp
        case(40):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                pregFile[laneID][pregDest[0]] = pregFile[laneID][pregSrc[0]] | pregFile[laneID][pregSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //xorp
        case(41):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) && (pregSrc[1]!=-1) ){
                pregFile[laneID][pregDest[0]] = pregFile[laneID][pregSrc[0]] ^ pregFile[laneID][pregSrc[1]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //notp
        case(42):{
            if( (pregDest[0]!=-1) && (pregSrc[0]!=-1) ) {
                pregFile[laneID][pregDest[0]] = !pregFile[laneID][pregSrc[0]];
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            
            break;
        }
            //isneg
        case(43):{
            if( (gprSrc[0]!=-1) && (pregDest[0]!=-1) ){
                if(((gprFile[laneID][gprSrc[0]] >> ((sizeof(T)*CHAR_BIT)-1))) == 1){
                    pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    pregFile[laneID][pregDest[0]] = false;
                }
                nextProgramCounter = programCounter + simdSpec->instLength;
                return true;
            }
            break;
        }
            //iszero
        case(44):{
            if( (gprSrc[0]!=-1) && (pregDest[0]!=-1) ){
                if((gprFile[laneID][gprSrc[0]]) == 0){
                    pregFile[laneID][pregDest[0]] = true;
                }
                else{
                    pregFile[laneID][pregDest[0]]  = false;
                }
                nextProgramCounter = programCounter + simdSpec->instLength;
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
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //jmpru
        case(47):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //skep
        case(48):{
            nextProgramCounter  = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //reti
        case(49):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //tlbrm
        case(50):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //itof
        case(51):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //ftoi
        case(52):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fadd
        case(53):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] + gprFile[laneID][gprSrc[1]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fsub
        case(54):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] - gprFile[laneID][gprSrc[1]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fmul
        case(55):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] * gprFile[laneID][gprSrc[1]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fdiv
        case(56):{
            if( (gprSrc[0]!=-1) &&  (gprSrc[1]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] / gprFile[laneID][gprSrc[1]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //fneg
        case(57):{
            if( (gprSrc[0]!=-1) && (gprDest[0]!=-1) ){
                gprFile[laneID][gprDest[0]] = gprFile[laneID][gprSrc[0]] / gprFile[laneID][gprSrc[1]];
            }
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //wspawn
        case(58):{
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
            break;
        }
            //split
        case(59):{
            //nextProgramCounter = programCounter + simdSpec->instLength;
            
            splitSign = true;
            
            //predSign[laneID] = (!predicateBit || pregFile[laneID][predReg]) && currMask[laneID];
            
            return true;
            break;
        }
            //join
        case(60):{
            
            joinSign = true;
            
            return true;
            break;
        }
            //bar
        case(61):{
            nextProgramCounter = programCounter + simdSpec->instLength;
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

template<typename T>
bool SimdCore<T>::execute(uint64_t opCodeValue, uint64_t laneID, bool debug){
    bool retVal = execute(opCodeValue, laneID);
    if(debug){
//        std::cerr << "\n-------------------------\n";
//        std::cerr << "Executing\n";
//        std::cerr << "---------------------------\n";
    }
    return retVal;
}

//simd executor
template<typename T>
void SimdCore<T>::start(bool debug){
    
    //Local control variables
	bool executeNext = true;
    T instruction;
    uint64_t opCodeVal =0;
    
    //Execute till loop
	while(executeNext){
        
        //debug Counter
        debug_counter++;
        
        //PC =  NPC
        programCounter =nextProgramCounter;
        
        if (programCounter== 0xac8) {
            int a =10;
        }
        
        //execute Next
        executeNext = false;
        
        //Curr Mask = Next Mask
        for (uint64_t count =0; count < simdSpec->simdLaneSize; count++) {
            currMask[count] = nextActMask[count];
        }
        
        resetOperands();
        
        if (debug_counter > 4418864) {
            instruction = fetch(true);
            opCodeVal = decode(instruction, true);
        }
        else{
            instruction = fetch(false);
            opCodeVal = decode(instruction, false);
        }
        
        
        bool executedOnce = false;
        
        //Execute Per lane
        for (uint64_t lane=0; lane < simdSpec->simdLaneSize; lane++) {
            if (currMask[lane]) {
                executedOnce = true;
                executeNext = this->execute(opCodeVal, lane, debug);
            }
        }
        
        //Check if not executed once
        if(!executedOnce){
            //PC
            nextProgramCounter = programCounter + simdSpec->instLength;
            executeNext = true;
        }
        
        //Check if last execution was split
        if (splitSign || (opCodeVal == 59)) {
            
            //PC
            nextProgramCounter = programCounter + simdSpec->instLength;
            
            executeNext = true;
            
            //Make split sign false
            splitSign = false;
            
            //Temp stack elem
            reconvStackElem<T> elem1;
            
            //Push arbit PC and curr mask
            elem1.nextPC = -1;
            elem1.activityMask = new bool[simdSpec->simdLaneSize];
            for(uint64_t count = 0 ; count < simdSpec->simdLaneSize; count++){
                elem1.activityMask[count] = currMask[count];
            }
            reconvStack.push(elem1);
            
            //Temp stack elem
            reconvStackElem<T> elem2;
            
            //Push Next PC and ~next mask
            elem2.nextPC = nextProgramCounter;
            elem2.activityMask = new bool[simdSpec->simdLaneSize];
            for (uint64_t predC=0; predC < simdSpec->simdLaneSize; predC++) {
                predSign[predC] = (!predicateBit || pregFile[predC][predReg]) && currMask[predC];
            }
            
            for (uint64_t count =0 ; count < simdSpec->simdLaneSize; count++) {
                elem2.activityMask[count] = (currMask[count] && !predSign[count]);
            }
            
            reconvStack.push(elem2);
            
            //Change next mask
            for (uint64_t count =0 ; count < simdSpec->simdLaneSize; count++) {
                nextActMask[count] = currMask[count] && predSign[count];
            }
            
            //Set predicate signature to 0
            for (uint64_t pC=0; pC < simdSpec->simdLaneSize; pC++) {
                predSign[pC] = false;
            }

        }
        
        //check if join was executed last
        if(joinSign || (opCodeVal == 0x3c)){
            joinSign = false;
            
            executeNext = true;
            
            if(reconvStack.top().nextPC!=-1){
                nextProgramCounter = reconvStack.top().nextPC;
            }
            else{
                nextProgramCounter = programCounter + simdSpec->instLength;
            }
            
            for(uint64_t count =0; count<simdSpec->simdLaneSize; count++){
                nextActMask[count]  = reconvStack.top().activityMask[count];
            }
            reconvStack.pop();
        }

        if(debug_counter >  4418864){
        if (debug) {
            
            std::cerr << std::hex <<    "\nRegister File:";
            for (uint64_t gprCount=0; gprCount < simdSpec->gprNum; gprCount++) {
                std::cerr << std::hex << "\nr"<<gprCount<<"\t";
                for(uint64_t laneCount =0; laneCount < gprFile.size(); laneCount++){
                    std::cerr << std::hex << "0x" << gprFile[laneCount][gprCount] << " ";
                }
            }
            
            std::cerr << std::hex <<    "\n\nPreg Register File:";
            
            for (uint64_t pregCount=0; pregCount < simdSpec->pregNum; pregCount++) {
                std::cerr << std::hex << "\npreg"<<pregCount<<"\t";
                for(uint64_t laneCount =0; laneCount < pregFile.size(); laneCount++){
                    std::cerr << std::hex << "0x" << pregFile[laneCount][pregCount] << " ";
                }
            }
            
            std::cerr << std::hex <<    "\n\nActivity Mask File:\n";
            
            std::cerr << std::hex <<    "------------------------------------\n";
            for (uint64_t maskCount =0 ; maskCount < simdSpec->simdLaneSize; maskCount++) {
                std::cerr << std::hex << " | " << currMask[maskCount];
            }
            std::cerr << std::hex <<    " |\n------------------------------------\n";
            
            
            if(!reconvStack.empty()){
                std::cerr << std::hex <<    "\n\nReconvergence Stack top Program Counter:\n";
                std::cerr << std::hex << reconvStack.top().nextPC;
                
                std::cerr << std::hex <<    "\n\nReconvergence Stack top Mask:\n";
                
                std::cerr << std::hex <<    "------------------------------------\n";
                for (uint64_t maskCount =0 ; maskCount < simdSpec->simdLaneSize; maskCount++) {
                    std::cerr << std::hex << " | " << reconvStack.top().activityMask[maskCount];
                }
    
                std::cerr << std::hex <<    " |\n------------------------------------\n";
                std::cerr << std::hex << "Stack Size: " << reconvStack.size() << "\n" ;
                
            }
   
            std::cerr << "\nInst Num: "<< debug_counter << "\n";
            
            std::cerr << "-----------------------------------" << "\n";

        }}
//        
        //std::cerr << std::hex << "0x" << programCounter << "\n";
        
        if(debug_counter == 4418867){
            int ab =10;
       }

        instruction = 0;
    }
}

//Actually the whole assignment
template<typename T>
bool SimdCore<T>::execute(T instruction, bool debug, uint64_t laneId){
    
    debug_counter++;
    
	uint32_t opcodeValue =0;
	T* gprInput1 	= nullptr;
	T* gprInput2 	= nullptr;
	T* gprOut		= nullptr;
    bool* pregIn1		= nullptr;
	bool* pregIn2		= nullptr;
	bool* pregOut		= nullptr;
	T immediate		= 0;
	ArgumentEnum_t	opcodeArg;

	bool predicateBit = (instruction >> ((simdSpec->predicate.position)-1));

	//Normal Execution

	//Get Opcode
	uint32_t opcodeBitsBefore 	= sizeof(T)*CHAR_BIT-simdSpec->opcode.position;
	uint32_t opcodeBitsAfter 	= simdSpec->opcode.position - simdSpec->opcode.length;
	opcodeValue = ((instruction << opcodeBitsBefore) >> opcodeBitsBefore) >> opcodeBitsAfter;

	//Get Arguments
	opcodeArg = simdSpec->opcodeToArgument[opcodeValue];

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

    
    if (debug_counter == 116480) {
        int a;
        a=10;
    }


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
				gprOut 					= gprFile[laneId] + gprNum1;
				gprInput1 				= gprFile[laneId] + gprNum2;
				break;
			}
			case(ArgumentEnum::AC_2IMM):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->gprBitLength);
				gprNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->gprBitLength;
				immVal			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1);
				gprOut 			= gprFile[laneId] + gprNum1;
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
				gprOut 					= gprFile[laneId] + gprNum1;
				gprInput1 				= gprFile[laneId] + gprNum2;
				gprInput2				= gprFile[laneId] + gprNum3;
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
                pregOut 				= pregFile[laneId] + pregNum1;
                pregIn1 				= pregFile[laneId] + pregNum2;
                pregIn2                 = pregFile[laneId] + pregNum3;
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
				gprOut 			= gprFile[laneId] + gprNum1;
				gprInput1 		= gprFile[laneId] + gprNum2;
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
				gprOut 			= gprFile[laneId] + gprNum1;
				gprInput1 		= gprFile[laneId] + gprNum2;
				gprInput2		= gprFile[laneId] + gprNum3;
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
				gprInput1 		= gprFile[laneId] + gprNum1;
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
				gprInput1 		= gprFile[laneId] + gprNum1;
                gprInput2		= gprFile[laneId] + gprNum2;
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
				leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
				gprNum1			= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;

				pregOut					= pregFile[laneId] + pregNum1;
				gprInput1 				= gprFile[laneId]  + gprNum1;
				break;
			}

			case(ArgumentEnum::AC_2PREG):{
				rightShiftVal1 	= (simdSpec->opcode.position - simdSpec->opcode.length - simdSpec->pregBitLength);
				rightShiftVal2 	= rightShiftVal1 - simdSpec->pregBitLength;
				pregNum1 		= opcodeWithArgs >> rightShiftVal1;
				leftShiftVal1 	= regShiftLeftVal + simdSpec->pregBitLength;
				pregNum2		= ((opcodeWithArgs << leftShiftVal1) >> leftShiftVal1) >> rightShiftVal2;

				pregOut			= pregFile[laneId] 	+ pregNum1;
				pregIn1 		= pregFile[laneId]  + pregNum2;
				break;
			}
			default:{
				return false;
				break;
			}
		}
    
   

		
    
    //Predicated check
    uint64_t predRegNum0 =  ((instruction << (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (sizeof(T)*CHAR_BIT-simdSpec->pregRegField.position)) >> (simdSpec->pregRegField.position - simdSpec->pregRegField.length);
    if(predicateBit){
        if(pregFile[laneId][predRegNum0]==false){
            nextProgramCounter = programCounter + simdSpec->instLength;
            return true;
        }
    }
    
    if(debug){
        std::cerr << "----------------------------------------------------------" << "\n";
        std::cerr << std::hex <<	"Instruction:\t0x" << instruction << "\n";
        std::cerr << std::hex <<	"Programcounter:\t0x" << programCounter << "\n";
        std::cerr << std::hex <<    "------------------------------------------------------\n";
        std::cerr << std::hex <<    "| " << predicateBit << " | " << predRegNum0 << " | "<< opcodeValue << " |\n";
        std::cerr << std::hex <<    "------------------------------------------------------\n";
        
        std::cerr << std::hex << 	"GPR Values:" << "GPR1: " << gprNum1 <<  " GPR2: " << gprNum2 << " GPR3: " << gprNum3 << "\n";
        std::cerr << std::hex << 	"PREG Values:" << "PREG1: " << pregNum1 <<  "PREG2: " << pregNum2 << "\n";
        std::cerr << std::hex << 	"Immediate Value:" << immVal << "\n";
        std::cerr << std::hex <<    "\nRegister File:";
        for (uint64_t gprCount=0; gprCount < simdSpec->gprNum; gprCount++) {
            std::cerr << std::hex << "\nr"<<gprCount<<"\t";
            for(uint64_t laneCount =0; laneCount < gprFile.size(); laneCount++){
                std::cerr << std::hex << "0x" << gprFile[laneCount][gprCount] << " ";
            }
        }
        
         std::cerr << std::hex <<    "\n\nPreg Register File:";
        
        for (uint64_t pregCount=0; pregCount < simdSpec->pregNum; pregCount++) {
            std::cerr << std::hex << "\npreg"<<pregCount<<"\t";
            for(uint64_t laneCount =0; laneCount < pregFile.size(); laneCount++){
                std::cerr << std::hex << "0x" << pregFile[laneCount][pregCount] << " ";
            }
        }
        
        std::cerr << std::hex <<    "\n\nActivity Mask File:\n";
        
        std::cerr << std::hex <<    "------------------------------------\n";
        for (uint64_t maskCount =0 ; maskCount < simdSpec->simdLaneSize; maskCount++) {
            std::cerr << std::hex << " | " << currMask[maskCount];
        }
        std::cerr << std::hex <<    " |\n------------------------------------\n";
        
        
        if(!reconvStack.empty()){
            std::cerr << std::hex <<    "\n\nReconvergence Stack top Program Counter:\n";
            std::cerr << std::hex << reconvStack.top().nextPC;
            
            std::cerr << std::hex <<    "\n\nReconvergence Stack top Mask:\n";
            
            std::cerr << std::hex <<    "------------------------------------\n";
            for (uint64_t maskCount =0 ; maskCount < simdSpec->simdLaneSize; maskCount++) {
                std::cerr << std::hex << " | " << reconvStack.top().activityMask[maskCount];
            }
            
            std::cerr << std::hex <<    " |\n------------------------------------\n";

        }
       
        
        
        std::cerr << "\nInst Num: "<< debug_counter << "\n";
        
        std::cerr << "----------------------------------------------------------" << "\n";
    }

		switch(opcodeValue){
			//NOP
			case(0):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//di
			case(1):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//ei
			case(2):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//tlbadd
			case(3):{
				nextProgramCounter = programCounter + simdSpec->instLength;
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
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//not
			case(6):{
				if(gprInput1 && gprOut){
					*gprOut = ~(*gprInput1);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//and
			case(7):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) & (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//or
			case(8):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) | (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//xor
			case(9):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) ^ (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//add
			case(10):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) + (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//sub
			case(11):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) - (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//mul
			case(12):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) * (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//div
			case(13):{
				if(gprInput1 && gprInput2 && gprOut){
					if((*gprInput2)!=0){
						*gprOut = (*gprInput1) / (*gprInput2);
						nextProgramCounter = programCounter + simdSpec->instLength;
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
						nextProgramCounter = programCounter + simdSpec->instLength;
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
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//shr
			case(16):{
				if(gprInput1 && gprInput2 && gprOut){
					*gprOut = (*gprInput1) >> (*gprInput2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//andi
			case(17):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) & (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//ori
			case(18):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) | (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//xori
			case(19):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) ^ (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//addi
			case(20):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) + (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//subi
			case(21):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) - (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//muli
			case(22):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) * (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//divi
			case(23):{
				if(gprInput1 && gprOut){
					if((immediate)!=0){
						*gprOut = (*gprInput1) /(immediate);
						nextProgramCounter = programCounter + simdSpec->instLength;
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
					if((immediate)!=0){
						*gprOut = (*gprInput1) % (immediate);
						nextProgramCounter = programCounter + simdSpec->instLength;
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
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//shri
			case(26):{
				if(gprInput1 && gprOut){
					*gprOut = (*gprInput1) >> (immediate);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}
				break;
			}
			//jali
			case(27):{
				if(gprOut){
					*gprOut = programCounter + simdSpec->instLength;
					nextProgramCounter = programCounter + (immediate + simdSpec->instLength);
					return true;
				}
				break;
			}
			//jalr
			case(28):{
				if(gprInput1 && gprOut){
					T prevProgramCounter = programCounter;
					*gprOut = programCounter + simdSpec->instLength;
					nextProgramCounter =  (*gprInput1);
					if(nextProgramCounter == programCounter){
						return false;
					}
					return true;
				}
				break;
			}
			//jmpi
			case(29):{
				T prevProgramCounter = programCounter;
				nextProgramCounter = programCounter + (immediate + simdSpec->instLength);
				if(programCounter == nextProgramCounter){
					return false;
				}
				return true;
				break;
			}

			//jmpr
			case(30):{
				if(gprInput1){
					T prevProgramCounter = programCounter;
					nextProgramCounter =  (*gprInput1);
					if(programCounter == nextProgramCounter){
						return false;
					}
					return true;
				}
				break;
			}
			//clone
			case(31):{
				nextProgramCounter = programCounter + simdSpec->instLength;
                if(gprInput1){
                    for(int i=0 ; i< simdSpec->gprNum ; i++){
                        gprFile[*gprInput1][i] = gprFile[laneId][i];
                    }
                    for(int i=0 ; i< simdSpec->pregNum ; i++){
                        pregFile[*gprInput1][i] = pregFile[laneId][i];
                    }
                    
                    return true;
                }
				
				break;
			}
			//jalis
			case(32):{
				
                if(gprOut && gprInput1){
                    *gprOut = programCounter + simdSpec->instLength;
                    nextProgramCounter = programCounter + (immediate + simdSpec->instLength);
                    for (uint64_t i =0 ; i < *gprInput1; i++) {
                        nextActMask[i] = true;
                    }
                    for (uint64_t i = *gprInput1; i < simdSpec->simdLaneSize; i++) {
                        nextActMask[i] = false;
                    }
                    return true;
                }
				break;
			}
			//jalrs
			case(33):{
                if(gprOut && gprInput1 && gprInput2){
                    *gprOut = programCounter + simdSpec->instLength;
                    nextProgramCounter = *gprInput2;
                    for (uint64_t i =0 ; i < *gprInput1; i++) {
                        nextActMask[i] = true;
                    }
                    for (uint64_t i = *gprInput1; i < simdSpec->simdLaneSize; i++) {
                        nextActMask[i] = false;
                    }
                    return true;
                }
				break;
			}
			//jmprt
			case(34):{
                if(gprInput1){
                    *gprOut = programCounter + simdSpec->instLength;
                    nextProgramCounter = *gprInput1;
                    nextActMask[laneId] = true;
                    for (uint64_t i = 0; i < simdSpec->simdLaneSize; i++) {
                        if(i!=laneId)
                            nextActMask[i] = false;
                    }
                    return true;
                }
				break;
			}
			//ld
			case(35):{
				if(gprOut && gprInput1){
					T actualAddr = (*gprInput1) + (immediate);
                    T temp_data =0;
                    for (uint64_t count =0; count < simdSpec->instLength; count++) {
                        uint64_t tempAddr = actualAddr + simdSpec->instLength -1 -count;
                        
                        if(memoryMap->memoryBuff.find((uint64_t)tempAddr) == memoryMap->memoryBuff.end()){
                            memoryMap->memoryBuff[(uint64_t)tempAddr] =  0;
                        }
                        temp_data = temp_data << CHAR_BIT | (uint64_t)memoryMap->memoryBuff[tempAddr];
                        
                    }
					*gprOut = temp_data;
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//st
			case(36):{
				if(gprInput1 && gprInput2){
					T actualAddr = (*gprInput2) + (immediate);

					if(actualAddr==simdSpec->writeAddr && laneId == 0){
						outputMemory.push_back((char)(*gprInput1));
					}
					else{
                        T tempAddr = actualAddr;
                        for (uint64_t count =0; count < simdSpec->instLength; count++) {
                            memoryMap->memoryBuff[(uint64_t)tempAddr] = (uint8_t)(*(gprInput1) >> (CHAR_BIT*count));
                            tempAddr++;
                        }
						
					}
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//ldi
			case(37):{
				if(gprOut){
					*gprOut = immediate;
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//rtop
			case(38):{
				if(gprInput1&&pregOut){
					if(*gprInput1){
						*pregOut = true;
					}
                    else{
                        *pregOut = false;
                    }
                    nextProgramCounter  = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//andp
			case(39):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) & (*pregIn2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//orp
			case(40):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) | (*pregIn2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//xorp
			case(41):{
				if(pregIn1&&pregIn2&&pregOut){
					*pregOut = (*pregIn1) ^ (*pregIn2);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//notp
			case(42):{
				if(pregIn1&&pregOut){
					*pregOut = !(*pregIn1);
					nextProgramCounter = programCounter + simdSpec->instLength;
					return true;
				}

				break;
			}
			//isneg
			case(43):{
				if(gprInput1&&pregOut){
					if(((*gprInput1 >> ((sizeof(T)*CHAR_BIT)-1))) == 1){
						*pregOut = true;
					}
                    else{
                        *pregOut = false;
                    }
                    nextProgramCounter = programCounter + simdSpec->instLength;
                    return true;
				}
				break;
			}
			//iszero
			case(44):{
				if(gprInput1&&pregOut){
					if((*gprInput1) == 0){
						*pregOut = true;
					}
                    else{
                        *pregOut  = false;
                    }
                    nextProgramCounter = programCounter + simdSpec->instLength;
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
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//jmpru
			case(47):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//skep
			case(48):{
				nextProgramCounter  = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//reti
			case(49):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//tlbrm
			case(50):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//itof
			case(51):{
				if(gprOut&&gprInput1){
					*gprOut = *gprInput1;
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//ftoi
			case(52):{
				if(gprOut&&gprInput1){
					*gprOut = *gprInput1;
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//fadd
			case(53):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = *gprInput1 + *gprInput2;
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//fsub
			case(54):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = *gprInput1 - *gprInput2;
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//fmul
			case(55):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) * (*gprInput2);
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//fdiv
			case(56):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) / (*gprInput2);
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//fneg
			case(57):{
				if(gprOut&&gprInput1&&gprInput2){
					*gprOut = (*gprInput1) / (*gprInput2);
				}
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//wspawn
			case(58):{
				nextProgramCounter = programCounter + simdSpec->instLength;
				return true;
				break;
			}
			//split
			case(59):{
                splitSign = true;
                return true;
				break;
			}
			//join
			case(60):{
                
                joinSign = true;
                
				return true;
				break;
			}
			//bar
			case(61):{
				nextProgramCounter = programCounter + simdSpec->instLength;
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

//Memory output for a SIMD lane
template <typename T>
std::string SimdCore<T>::getOutputData(){
    std::string myStr;
    for (uint64_t i=0; i<outputMemory.size(); i++) {
        myStr += outputMemory[i];
    }
 
    return myStr;
}

//Clear GPR register File
template <typename T>
void SimdCore<T>::clearGprFile(){
    for (uint64_t i = 0 ; i < simdSpec->simdLaneSize; i++) {
        for (uint64_t j =0 ; j < simdSpec->gprNum; j++) {
             gprFile[i][j]  =   DEFAULT_GPRVAL;
        }
    }
}

//Clear PREG register file
template <typename T>
void SimdCore<T>::clearPregFile(){
    for (uint64_t i = 0; i < simdSpec->simdLaneSize ; i++) {
        for (uint64_t j =0 ; j < simdSpec->pregNum; j++) {
            pregFile[i][j] = DEFAULT_PREGVAL;
        }
    }
}

//Clear Program counter
template <typename T>
void SimdCore<T>::clearProgramCounter(){
    programCounter = 0;
    nextProgramCounter =0;
}


//Clear Output Memory counter
template <typename T>
void SimdCore<T>::clearOutPutMemory(){
    outputMemory.resize(0);
    outputMemory.shrink_to_fit();
}

template <typename T>
void SimdCore<T>::clearActivityMask(){
    int maskVal = DEFAULT_ACTIVITYMSK;
    for (int i=0; i < simdSpec->simdLaneSize; i++) {
        if (maskVal&(1<<i)) {
            currMask[i] = true;
            nextActMask[i] = true;
        }
        else{
            currMask[i] = false;
            nextActMask[i] = false;
        }
    }
}

//Reset the object
template <typename T>
void SimdCore<T>::reset(){
    clearGprFile();
    clearPregFile();
    clearProgramCounter();
    clearActivityMask();
    clearOutPutMemory();
    resetOperands();
}

template <typename T>
void SimdCore<T>::executeNextPC(){
    
}


//Copy constructor
template <typename T>
SimdCore<T>::SimdCore(const SimdCore& other){
    simdSpec 		= other.simdSpec;
    memoryMap 		= other.memoryMap;
    for (uint64_t i=0; i < simdSpec->simdLaneSize; i++) {
        gprFile.push_back(new T[simdSpec->gprNum]);
        pregFile.push_back(new bool[simdSpec->pregNum]);
    }
    
    for (uint64_t i=0; i < simdSpec->simdLaneSize; i++){
        for (uint64_t j=0; j < simdSpec->gprNum; j++) {
            gprFile[i][j] = other.gprFile[i][j];
        }
    }
    
    for (uint64_t i=0; i < simdSpec->simdLaneSize; i++){
        for (uint64_t j=0; j < simdSpec->pregNum; j++) {
            pregFile[i][j] = other.pregFile[i][j];
        }
    }
    
    programCounter	= other.programCounter;
    debug_counter = other.debug_counter;
}

template <typename T>
SimdCore<T>::~SimdCore(){
    for (uint64_t count =0; count < simdSpec->simdLaneSize; count++) {
        delete[] gprFile[count];
        delete[] pregFile[count];
    }
    
}

template class SimdCore<unsigned int>;
template class SimdCore<unsigned long long>;

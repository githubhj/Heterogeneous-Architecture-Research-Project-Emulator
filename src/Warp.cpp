/*
 * Warp.cpp
 *
 *  Created on: Oct 31, 2015
 *      Author: harshitjain
 */

#include "Warp.h"

//Default Constructor
template <typename T>
Warp<T>::Warp():debugFlag(false),simdCoreSize(DEFAULT_LANE_SIZE),warpId(0),debug_counter(0),programCounter(0),nextProgramCounter(0){

	//Init gpr and Preg
	initGPR(DEFAULT_GPR_SIZE, simdCoreSize);
	initPreg(DEFAULT_PREG_SIZE, simdCoreSize);
	initActMask(DEFAULT_LANE_SIZE);
}

//Actual Constructor
template <typename T>
Warp<T>::Warp(ArchSpec_t* simdPtr, uint64_t _warpId, bool _debugFlag){

	//Init gpr and Preg
	initActMask(simdPtr->simdLaneSize);
	initGPR(simdPtr->gprNum, simdPtr->simdLaneSize);
	initPreg(simdPtr->pregNum, simdPtr->simdLaneSize);

	//Init local variables
	warpId = _warpId;
	debugFlag = _debugFlag;
	debug_counter = 0;
	simdCoreSize = simdPtr->simdLaneSize;
	gprSize = simdPtr->gprNum;
	pregSize = simdPtr->pregNum;
	nextProgramCounter = 0;
	programCounter = 0;
    isExecuting = false;

}

//Destructor
template <typename T>
Warp<T>::~Warp() {
	// TODO Auto-generated destructor stub
	delete[] currMask;
	delete[] nextActMask;
}

//Allocate and init activity mask
template <typename T>
void Warp<T>::initActMask(uint64_t laneSize){
	currMask 	= new bool[laneSize];
	nextActMask = new bool[laneSize];
    shadowMask = new bool[laneSize];
	currMask[0]		= DEFAULT_ACTIVITYMSK;
	nextActMask[0]	= DEFAULT_ACTIVITYMSK;
    shadowMask[0] = false;
	for(uint64_t laneID = 1 ; laneID < laneSize; laneID++){
		currMask[laneID] 	= false;
		nextActMask[laneID] = false;
        shadowMask[laneID] = false;
        
	}
}

//Allocate and initialize GPR
template <typename T>
void Warp<T>::initGPR(uint64_t fileSize, uint64_t laneSize){
	for(uint64_t laneID =0 ; laneID < laneSize ; laneID++){
        std::vector<T> tempGPR;
		for(uint64_t fileID=0; fileID < fileSize ; fileID++){
			tempGPR.push_back(DEFAULT_GPRVAL);
		}
        gprFile.push_back(tempGPR);
	}
}

//Allocate and initialize Preg
template <typename T>
void Warp<T>::initPreg(uint64_t fileSize, uint64_t laneSize){
	for(uint64_t laneID =0 ; laneID < laneSize ; laneID++){
        std::vector<bool> tempPREG;
        for(uint64_t fileID=0; fileID < fileSize ; fileID++){
            tempPREG.push_back(DEFAULT_PREGVAL);
        }
        pregFile.push_back(tempPREG);
	}
}

//Allocate and initialize PC
template <typename T>
void Warp<T>::initPC(){
	programCounter = 0;
	nextProgramCounter = 0;
}

//Allocate and initialize PC
template <typename T>
void Warp<T>::initPred(uint64_t laneSize){
    splitSign = false;
    joinSign  = false;
    predSign = new bool[laneSize];
    inBarrier = false;
    for(uint64_t laneCount=0; laneCount < laneSize ; laneCount++){
        predSign[laneCount] = DEFAULT_PREGVAL;
    }
}

template <typename T>
void Warp<T>::printActivityMask(){
	
    std::cerr << std::hex <<    "\n\nActivity Mask for WarpID: " << warpId << "\n";
    std::cerr << std::hex <<    "----------------------------------\n";
    for (uint64_t maskCount =0 ; maskCount < simdCoreSize; maskCount++) {
        std::cerr << std::hex << "| " << currMask[maskCount] << " ";
    }
	std::cerr << std::hex <<    " |\n----------------------------------\n";
	
}

template <typename T>
void Warp<T>::printGPR(){
    std::cerr << std::hex << "\nRegister File for WarpID: " << warpId << "\n" ;
    for (uint64_t gprCount=0; gprCount < gprSize; gprCount++) {
        std::cerr << std::hex << "\nr"<<gprCount<<"\t";
        for(uint64_t laneCount =0; laneCount < simdCoreSize; laneCount++){
            std::cerr << std::hex << "0x" << gprFile[laneCount][gprCount] << " ";
        }
    }
    
}

template <typename T>
void Warp<T>::printPreg(){
	std::cerr << std::hex <<    "\n\nPreg Register File: " << warpId << "\n";

	for (uint64_t pregCount=0; pregCount < pregSize; pregCount++) {
		std::cerr << std::hex << "\npreg"<<pregCount<<"\t";
 		for(uint64_t laneCount =0; laneCount < simdCoreSize; laneCount++){
			std::cerr << std::hex << "0x" << pregFile[laneCount][pregCount] << " ";
		}
	}
}

template <typename T>
void Warp<T>::printStackContents(bool verbose){
    std::cerr << std::hex << "\n\n Stack Contents:\n";
    std::cerr << std::hex << "Stack Size: " << reconvStack.size() << "\n";
    if (verbose) {
        std::cerr << std::hex << "-----------------------------------\n";
        for (int64_t count = reconvStack.size()-1 ; count >=0; count--) {
            std::cerr << std::hex << "| PC |" << reconvStack[count].nextPC << " ";
            for (uint64_t mskC = 0; mskC < simdCoreSize; mskC++) {
                std::cerr << std::hex << "| " << reconvStack[count].activityMask[mskC] << " ";
            }
            std::cerr << std::hex << "|\n-----------------------------------\n";
        }
    }
    else{
        if (!reconvStack.empty()) {
            std::cerr << std::hex << "Stack Top Cotents:\n";
            std::cerr << std::hex << "-------------------------------------------\n";
            std::cerr << std::hex << "| PC |" << reconvStack[reconvStack.size()-1].nextPC << " ";
            
            for (uint64_t mskC = 0; mskC < simdCoreSize; mskC++) {
                std::cerr << std::hex << "| " << reconvStack[reconvStack.size()-1].activityMask[mskC] << " ";
            }
            std::cerr << std::hex << "|\n-------------------------------------------\n";
        }
    }
}

template <typename T>
void Warp<T>::printPC(){
    std::cerr << std::hex << "\n----------------------\n";
    std::cerr << std::hex << "| PC | " << programCounter << " | Steps | " << debug_counter<< " ";
    std::cerr << std::hex << "|\n----------------------\n";
}

template <typename T>
void Warp<T>::printStats(bool verbose){
    
    if (debugFlag) {
        std::cerr << std::hex << "\nWarp ID: " << warpId << "\n";
        printPC();
        printGPR();
        printPreg();
        printActivityMask();
        printStackContents(verbose);
    }
   
}


template <typename T>
void Warp<T>::reset(){
    // TODO Auto-generated destructor stub
    delete[] currMask;
    delete[] nextActMask;
    gprFile.clear();
    pregFile.clear();
	initActMask(simdCoreSize);
	initGPR(gprSize,simdCoreSize);
	initPreg(pregSize,simdCoreSize);
	initPC();
    isExecuting = false;
}

template class Warp<uint32_t>;
template class Warp<uint64_t>;




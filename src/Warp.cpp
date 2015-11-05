//============================================================================
// Name        : Warp.cpp
// Author      : Harshit Jain
// Version     : 1.0
// Copyright   : It's protected under Beerware
// GT ID       : 903024992
// Class       : ECE 8823
// Assignment  : 3
// Description : Warp Class in C++, Ansi-style
//============================================================================

#include "Warp.h"

/*Warp default constructor.
 * Initializes internals as per constants.
 * */
template <typename T>
Warp<T>::Warp():debugFlag(false),simdCoreSize(DEFAULT_LANE_SIZE),warpId(0),debug_counter(0),programCounter(0),nextProgramCounter(0){

	//Init gpr and Preg
	initGPR(DEFAULT_GPR_SIZE, simdCoreSize);
	initPreg(DEFAULT_PREG_SIZE, simdCoreSize);
	initActMask(DEFAULT_LANE_SIZE);
}

/*
 * Warp: Main constructor.
 * params:	1. ArchSpec_t: Pointer to arch specification.
 * 			2. Warp id.
 * 			3. Debug flag
 **/
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

/*
 * Warp		: Main destructor.
 * params	:	None
 **/
template <typename T>
Warp<T>::~Warp() {
	// TODO Auto-generated destructor stub
	delete[] currMask;
	delete[] nextActMask;
}

/*
 * Warp		: 	initMask routine, to allocate memory to thread mask and initialize them.
 * params	:	Lanesize of the warp.
 **/
template <typename T>
void Warp<T>::initActMask(uint64_t laneSize){
	//Get memory
	//Running mask
	currMask 	= new bool[laneSize];
	//Mask for next cycle
	nextActMask = new bool[laneSize];
	//Back-up mask to be used after barrier
    shadowMask = new bool[laneSize];

    //Set 0th Lane
	currMask[0]		= DEFAULT_ACTIVITYMSK;
	nextActMask[0]	= DEFAULT_ACTIVITYMSK;
    shadowMask[0] = false;

    //Set rest of the lanes as false
	for(uint64_t laneID = 1 ; laneID < laneSize; laneID++){
		currMask[laneID] 	= false;
		nextActMask[laneID] = false;
        shadowMask[laneID] = false;
        
	}
}

/*
 * Warp		: 	initGPR routine, to allocate memory to thread GPR file and initialize them.
 * params	:	1. GPR file size of a lane.
 * 				2. Lanesize of the warp.
 **/
template <typename T>
void Warp<T>::initGPR(uint64_t fileSize, uint64_t laneSize){

	//For each lane
	for(uint64_t laneID =0 ; laneID < laneSize ; laneID++){
		//For each GPR
        std::vector<T> tempGPR;
		for(uint64_t fileID=0; fileID < fileSize ; fileID++){
			tempGPR.push_back(DEFAULT_GPRVAL);
		}
        gprFile.push_back(tempGPR);
	}
}

/*
 * Warp		: 	initPREG routine, to allocate memory to thread PREG file and initialize them.
 * params	:	1. GPR file size of a lane.
 * 				2. Lanesize of the warp.
 **/
template <typename T>
void Warp<T>::initPreg(uint64_t fileSize, uint64_t laneSize){
	//For each lane
	for(uint64_t laneID =0 ; laneID < laneSize ; laneID++){
		//For each preg
        std::vector<bool> tempPREG;
        for(uint64_t fileID=0; fileID < fileSize ; fileID++){
            tempPREG.push_back(DEFAULT_PREGVAL);
        }
        pregFile.push_back(tempPREG);
	}
}

/*
 * Warp		: 	initPC routine, to initialize PC and nextPC to 0.
 * params	:	None.
 **/
template <typename T>
void Warp<T>::initPC(){
	programCounter = 0;
	nextProgramCounter = 0;
}

/*
 * Warp		: 	initPred routine, to initialize predsign used by split instruction.
 * 				Initialise Split flag, Join flag and Barrier flag.
 * params	:	Lane size.
 **/
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

/*
 * Warp		: printActivityMask(), pretty print for activity masks.
 **/
template <typename T>
void Warp<T>::printActivityMask(){
	
    std::cerr << std::hex <<    "\n\nActivity Mask for WarpID: " << warpId << "\n";
    std::cerr << std::hex <<    "----------------------------------\n";
    for (uint64_t maskCount =0 ; maskCount < simdCoreSize; maskCount++) {
        std::cerr << std::hex << "| " << currMask[maskCount] << " ";
    }
	std::cerr << std::hex <<    " |\n----------------------------------\n";
	
}

/*
 * Warp		: printGPR(), pretty print for GPR file.
 **/
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

/*
 * Warp		: printPreg(), pretty print for PREG file.
 **/
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

/*
 * Warp		: printStackContents(), pretty print for reconvergence stack.
 * params	: verbose: print complete stack if true, else only top.
 **/
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

/*
 * Warp		: printPC(), pretty print for PC and Step count.
 **/
template <typename T>
void Warp<T>::printPC(){
    std::cerr << std::hex << "\n----------------------\n";
    std::cerr << std::hex << "| PC | " << programCounter << " | Steps | " << debug_counter<< " ";
    std::cerr << std::hex << "|\n----------------------\n";
}


/*
 * Warp		: printStats(), pretty print for warp statistics.
 **/
template <typename T>
void Warp<T>::printStats(bool verbose){
    
	//If debug flag is set
    if (debugFlag) {
    	//Use all othe prints
        std::cerr << std::hex << "\nWarp ID: " << warpId << "\n";
        printPC();
        printGPR();
        printPreg();
        printActivityMask();
        printStackContents(verbose);
    }
   
}

/*
 * Warp		: reset(), reset and reassign everything.
 **/
template <typename T>
void Warp<T>::reset(){
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




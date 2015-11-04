/*
 * Warp.h
 *
 *  Created on: Oct 31, 2015
 *      Author: harshitjain
 */

#ifndef SRC_WARP_H_
#define SRC_WARP_H_

#include <cstdint>
#include <vector>
#include "MemoryMap.h"

#define DEFAULT_LANE_SIZE 	8UL
#define DEFAULT_GPR_SIZE  	8UL
#define DEFAULT_PREG_SIZE	8UL
#define DEFAULT_GPRVAL 0UL
#define DEFAULT_PREGVAL false
#define DEFAULT_ACTIVITYMSK true

//Reconvergence Stack element
template<typename T>
struct reconvStackElem {
    T nextPC;
    bool* activityMask;
};

typedef enum ArgumentEnum{AC_NONE, AC_2REG, AC_2IMM, AC_3REG,
    AC_3PREG, AC_3IMM, AC_3REGSRC, AC_1IMM,
    AC_1REG, AC_3IMMSRC, AC_PREG_REG ,AC_2PREG}ArgumentEnum_t;


typedef struct _instrField{
    uint32_t position;
    uint32_t length;
}instrField_t;

typedef struct _ArchSpec_t{
    //In Bytes
    uint32_t instLength;
    
    //In Numbers
    uint32_t gprNum;
    uint32_t gprBitLength;
    uint32_t pregNum;
    uint32_t pregBitLength;
    uint64_t writeAddr;
    uint64_t warpSize;
    uint64_t simdLaneSize;
    
    //Vector pointer for opcode to argument map
    std::vector<ArgumentEnum_t> opcodeToArgument;
    
    //Memory Map
    MemoryMap* memoryMap;
    
    //Instruction Fields
    instrField_t predicate;
    instrField_t pregRegField;
    instrField_t opcode;
    
}ArchSpec_t;


template <typename T>
class Warp {

	//Simd Core Size
	uint64_t			simdCoreSize;

	//GPRFile Size
	uint64_t gprSize;

	//PREG File Size
	uint64_t pregSize;

    //Allocate and Initialize GPR
	void initGPR(uint64_t fileSize, uint64_t laneSize);
    
    //Allocate and initialize Preg
	void initPreg(uint64_t fileSize, uint64_t laneSize);
    
    //Allocate and initialize activity mask
	void initActMask(uint64_t laneSize);
    
    //Allocate and initialize predicate
    void initPred(uint64_t laneSize);
    
    //Initiaze PC
	void initPC();

public:

	//Re-convergence Stack
	std::vector<reconvStackElem<T> >	reconvStack;

	//Current Program Counter for warp
	T 					programCounter;

	//Next Program Counter for Warp
	T					nextProgramCounter;

	//GPR File
    std::vector<std::vector<T> > 	gprFile;

	//PregFile
    std::vector<std::vector<bool> > pregFile;

	//Current Activity Mask
	bool*				currMask;
    
    //Shadow Mask for barrriers
    bool*               shadowMask;

	//Activity Mask for Next Cycle
	bool*				nextActMask;

	//WarpId
	uint64_t			warpId;

	//Debug Counter or Steps of execution in this warp
	uint64_t			debug_counter;
    
    //Split Join control
    bool                splitSign;
    bool                joinSign;
    bool*               predSign;
    bool                inBarrier;
    
    //execution flag: True means its is executing and false means its halted
    bool isExecuting;

    //Debug Flag
    bool				debugFlag;

	//Initialize warp as per default parameters
	Warp();

	//Constructor for given warp size
	Warp(ArchSpec_t*,uint64_t,bool);

	//Print Values for debug mode
	void printGPR();
	void printPreg();
	void printPC();
	void printActivityMask();
	void printStackContents(bool verbose);
    void printStats(bool verbose);

	//Reset Warp
	void reset();

	virtual ~Warp();
};


#endif /* SRC_WARP_H_ */

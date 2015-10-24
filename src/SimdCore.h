/*
 * SimdCore.h
 *
 *  Created on: Sep 24, 2015
 *      Author: harshitjain
 */

#ifndef SIMDCORE_H_
#define SIMDCORE_H_

#include <iostream>
#include <cstdlib>
#include <vector>
#include <limits>
#include <cmath>
#include <map>
#include "MemoryMap.h"
#include <stack>

#define DEFAULT_GPRVAL 0UL
#define DEFAULT_PREGVAL false

//Reconvergence Stack element
template<typename T>
struct reconvStackElem {
    T nextPC;
    bool* activityMask;
};

typedef std::vector<char> outputmemory_t;

typedef struct _instrField{
	uint32_t position;
	uint32_t length;
}instrField_t;

typedef enum ArgumentEnum{AC_NONE, AC_2REG, AC_2IMM, AC_3REG,
							AC_3PREG, AC_3IMM, AC_3REGSRC, AC_1IMM,
							AC_1REG, AC_3IMMSRC, AC_PREG_REG ,AC_2PREG}ArgumentEnum_t;

typedef struct _archSpec{
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



class SimdCoreBase{
public:

	SimdCoreBase(){};
	virtual void start(bool debug)=0;
    virtual void executeNextPC()=0;
    virtual std::string getOutputData()=0;
    virtual void reset()=0;
    //outputmemory
    outputmemory_t 		outputMemory;
	virtual ~SimdCoreBase(){};
};

template <typename T>
class SimdCore:public SimdCoreBase{
private:
	//Architectural Specification
	ArchSpec_t* simdSpec;

	//Mmeory Map
	MemoryMap* memoryMap;
	
	//Program counter and register file
	T                       programCounter;
    T                       nextProgramCounter;
    std::vector<T*>         gprFile;
    std::vector<bool*> 		pregFile;
    
    //curr Mask
    bool* currMask;
    //Next Active Mask
    bool* nextActMask;
    
    //debug counter;
    uint64_t debug_counter;
    
    //Warp Stack
    std::stack<reconvStackElem<T> > reconvStack;

public:

	//Constructor
	SimdCore(ArchSpec_t*, MemoryMap*);

	//Copy constructor
	SimdCore(const SimdCore& other);

	//Assignment operator
	SimdCore& operator=(const SimdCore& rhs);
    
    //start the GPU
	void start(bool debug);
    
    //Get output memory into a string
    std::string getOutputData();

	bool execute(T instruction, bool debug, uint64_t laneID);
    
    void executeNextPC();
    
    void reset(void);
    void clearGprFile(void);
    void clearPregFile(void);
    void clearProgramCounter(void);
    void clearOutPutMemory(void);

	~SimdCore();
};


#endif /* SIMDCORE_H_ */

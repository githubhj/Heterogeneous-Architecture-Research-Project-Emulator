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
#include <vector>
#include <set>
#include "MemoryMap.h"
#include "Warp.h"
#include <limits.h>
#include <stdint.h>


typedef std::vector<char> outputmemory_t;

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
    
    //Instruction Operands
    uint64_t    gprSrc[2];
    uint64_t    gprDest[1];
    uint64_t    pregSrc[2];
    uint64_t    pregDest[1];
    T           immediate;
    bool        predicateBit;
    uint64_t    predReg;
    
    
    //Reset Operands
    void resetOperands();
    
    //debug counter;
    uint64_t debug_counter;
    
    //Warp Activity MAsk
    bool* warpExecutionFlag;
    
    bool* predSign;
    
    std::vector<Warp<T>*> warpQueue;
    
    std::map<T,std::set<Warp<T>* > > barrier;
    
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
    
    //Fetch next instruction
    T fetch(T programCounter);
    T fetch(T programCounter, bool debug);
    
    //Decode
    uint64_t decode(T instruction);
    uint64_t decode(T instruction,bool debug);
    
    //Execute
    bool execute(uint64_t opCodeValue, uint64_t warpID, uint64_t laneID);
    bool execute(uint64_t opCodeValue, uint64_t warpID, uint64_t laneID, bool debug);
	bool execute(T instruction, bool debug, uint64_t laneID);
    
    void executeNextPC();
    
    void reset(void);
    void clearOutPutMemory(void);

	~SimdCore();
};


#endif /* SIMDCORE_H_ */

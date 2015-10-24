/*
 * Warp.h
 *
 *  Created on: Oct 21, 2015
 *      Author: harshitjain
 */

#ifndef SRC_WARP_H_
#define SRC_WARP_H_

#include <cstdint>
#include <stack>
#include "SimdCore.h"


//Reconvergence Stack element
template<typename T>
struct reconvStackElem {
    T nextPC;
    uint64_t maskSize;
    bool* activityMask;
};

//Warp Class
template<typename T>
class Warp {
private:
    //War Size
    uint64_t warpSize;
    //Warp Stack
    std::stack<reconvStackElem<T> > reconvStack;
    //Warp Lanes Pointers
    SimdCoreBase* lanes;
    //Current Mask
    bool* currMask;
    
public:
    //Default Constructor
    Warp();
    //Actual Constructor
    Warp(uint64_t _waprSize);
    //Push on reconverggence stack
    void reconvStackPush(reconvStackElem<T> elem);
    //Pop from reconvergence stack
    void reconvStackPop();
    //Get top of the stack
    reconvStackElem<T> reconvStackTop();
    //Check is reconvergence stack is empty
    bool reconvStackEmpty();
    //Execute in lock step
    void executeLockStep();
    //Assign lane pointers
    void assignLanes(SimdCoreBase*);
    //I am the DESTRUCTOR
	virtual ~Warp();
};

#endif /* SRC_WARP_H_ */

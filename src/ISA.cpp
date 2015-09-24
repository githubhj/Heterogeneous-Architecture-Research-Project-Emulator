/*
 * ISA.cpp
 *
 *  Created on: Sep 22, 2015
 *      Author: harshitjain
 */

#include "ISA.h"

ISA::ISA() : instLength(4),gprNum(8),pregNum(8),simdLanes(1),warpNum(1){


}

ISA::ISA(int iL,int gN, int pN, int sL, int wN){
	instLength = iL;
	gprNum = gN;
	pregNum = pN;
	simdLanes = sL;
	warpNum = wN;
}

ISA::~ISA() {
	// TODO Auto-generated destructor stub
}


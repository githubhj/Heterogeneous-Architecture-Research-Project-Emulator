/*
 * ISA.h
 *
 *  Created on: Sep 22, 2015
 *      Author: harshitjain
 */

#ifndef SRC_ISA_H_
#define SRC_ISA_H_

class ISA {
private:
	//In Bytes
	int instLength;
	//In Numbners
	int gprNum;
	int pregNum;
	int simdLanes;
	int warpNum;
public:
	ISA();
	ISA(int,int,int,int,int);
	virtual ~ISA();
};

#endif /* SRC_ISA_H_ */

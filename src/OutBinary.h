/*
 * OutBinary.h
 *
 *  Created on: Sep 23, 2015
 *      Author: harshitjain
 */

#ifndef SRC_OUTBINARY_H_
#define SRC_OUTBINARY_H_

#include <fstream>
#include <iostream>

class OutBinary {
	std::ofstream outputFile;
public:
	OutBinary();
	OutBinary(std::string);
	void write(unsigned char);
	virtual ~OutBinary();
};

#endif /* SRC_OUTBINARY_H_ */

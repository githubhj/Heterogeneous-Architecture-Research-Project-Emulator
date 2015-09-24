/*
 * OutBinary.cpp
 *
 *  Created on: Sep 23, 2015
 *      Author: harshitjain
 */

#include "OutBinary.h"

OutBinary::OutBinary() {
	// TODO Auto-generated constructor stub
	outputFile.open("output.out",std::ostream::binary|std::ostream::out);
}

OutBinary::OutBinary(std::string fileName){
	outputFile.open(fileName,std::ostream::binary|std::ostream::out);
	outputFile.seekp(0,outputFile.beg);
}

void OutBinary::write(unsigned char data){
	outputFile.write((char*)&data, sizeof(data));
}

OutBinary::~OutBinary() {
	// TODO Auto-generated destructor stub
	outputFile.close();
}


/*
 * GpuCore.h
 *
 *  Created on: Oct 5, 2015
 *      Author: harshitjain
 */

#ifndef SRC_GPUCORE_H_
#define SRC_GPUCORE_H_

#include "SimdCore.h"
#include "MemoryMap.h"

class GpuCoreBase{
	public:
	GpuCoreBase(){};
	virtual void start()=0;
	virtual void reset()=0;
	virtual ~GpuCoreBase(){};
};

template <typename T>
class GpuCore:public GpuCoreBase{
private:
	SimdCore<T>* simdCores;
public:
	//Take arch specification, memorymap pointer and lane size
	GpuCore(ArchSpec_t*,MemoryMap*,int);
	//Begin execution
	void start();
	//reset all simdcores
	void reset();
	virtual ~GpuCore();
};

#endif /* SRC_GPUCORE_H_ */

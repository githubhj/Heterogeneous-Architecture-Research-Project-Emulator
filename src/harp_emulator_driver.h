/*
 * harp_emulator_driver.h
 *
 *  Created on: Sep 26, 2015
 *      Author: harshitjain
 */

#ifndef HARP_EMULATOR_DRIVER_H_
#define HARP_EMULATOR_DRIVER_H_

#include <cinttypes>
#include <limits>
#include <cstdint>
#include <vector>
#include "SimdCore.h"
#include "MemoryMap.h"
#include <memory>
#include <stdint.h>

//Default value for execution
#define DEFAULT_IL 4
#define DEFAULT_GN 8
#define DEFAULT_PN 8
#define DEFAULT_SL 8
#define DEFAULT_WN 8

bool loadOpcodeToInstr(ArchSpec_t*);

typedef std::map<uint64_t, outputmemory_t> threadOutputMemory_t;

#endif /* HARP_EMULATOR_DRIVER_H_ */

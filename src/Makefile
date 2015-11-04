#### HarpTool MakeFile

INCLUDE 	= -I ./
CXXFLAGS  = -std=c++11 -Wall -Wno-constant-conversion
CXX     	= g++
RM      	= /bin/rm -rf


SRCS = harp_emulator_driver.cpp SimdCore.cpp MemoryMap.cpp Warp.cpp 
OUTPUT = harp_emulator
OBJS = $(subst %.cpp,%.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(OUTPUT) $(OBJS)

harp_emulator_driver.o: harp_emulator_driver.cpp harp_emulator_driver.h

SimdCore.o: SimdCore.cpp SimdCore.h

MemoryMap.o: MemoryMap.cpp MemoryMap.h

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) $(OUTPUT)

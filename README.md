# Heterogeneous-Architecture-Research-Project-Emulator
###An emulator for a subset of the HARP instruction set architecture, according to the HARP ISA documentation.
----------
An C++ based HARP emulator which emulates HARP ISA and provides execution model of HARP ISA based GPU.
HARP is a resrearch based instruction set architecture similar to MIPS but includes some instructions for control and branch divergence for a GPU-SIMD lane. Refer to /docs/harp_iset.pdf for more details.

##### /src
----------
1. harp_emulator_driver.cpp: Driver code taht takes command line values or parameters for GPU emulation. For example:
./harp_emulator <input binary file> <output file> <architecture width(4 or 8)> <register file size> <predicate register file size> <simd lane size> <warp number size>

./src/harp_emulator bubble.bin ../output/bubble.emu.out 
above command runs for default config: 4w/8/8/8/8

2. SimdCore.cpp: SimdCore class to create SISD object for SIMD execution.
3. MemoryMap.cpp: MemoryMap, copies the given bianry into internal data struction so as to allow SIMD execution from SIMD lanes.
4. To compile: do: make @ ./src
5. To run: Example: ./src/harp_emulator bubble.bin ../output/bubble.emu.out

##### /data
----------
Given data for execution asm to generate bianries and output for correctness evaluation.

##### /docs

Given docs for Assignment, harp ISA, harmoica ppt and Assignment pdf.




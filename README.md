# Heterogeneous-Architecture-Research-Project-Emulator
###An emulator for a subset of the HARP instruction set architecture, according to the HARP ISA documentation.
----------
An C++ based HARP emulator which emulates HARP ISA and provides execution model of HARP ISA based GPU.
HARP is a resrearch based instruction set architecture similar to MIPS but includes some instructions for control and branch divergence for a GPU-SIMD lane. Refer to /docs/harp_iset.pdf for more details.

##### /src
----------
1. harp_emulator_driver.cpp: Driver code that takes command line values or parameters for GPU emulation. For example:
./harp_emulator <input binary file> <output file> <architecture width(4 or 8)> <register file size> <simd lane size> <warp number size>

./src/harp_emulator bubble.bin ../output/bubble.emu.out 
above command runs for default config: 4w/8/8/8/8

2. SimdCore.cpp: SimdCore class to executed SIMD model of GPU, instantiates WARP Queue as per given warp size, controls fetch, decode and execution model. Also implements PDOM execution for split/join and wspawn and barrier for Simd control flow.
3. MemoryMap.cpp: MemoryMap, copies the given binary into internal data structure so as to allow SIMD execution from SIMD lanes.
4. Warp.cpp: Warp class to create warp objects with given lane size and GPR and PREG register file of given size. Includes execution mask for lanes and reconvergence stack for PDOm implementation.
5. To compile: do: make @ ./src
5. To run: Example: ./src/harp_emulator bubble.bin ../output/bubble.emu.out

##### /data
----------
Given data for execution, asm to generate bianries and output for correctness evaluation.

##### /docs

Given docs for Assignment, harp ISA, harmoica ppt and Assignment pdf.

##### /output

Final output runs for submitted verisons.

#### /time

Time outputs of test run before submission, gives an approximate time of execution of submitted simulator for different binaries. From results: hastable and binsearch execute in minutes while vecsum and bfs might take 60-70  minutes. I executed on Macbook-Pro with 2.9 GHz Intel Core i5, with 8 GB 1867 MHz DDR3.

#### Remarks

I have tested the makefile on pace cluster. It is able to compile but I was unable to check the correctness of my simulator as pace cluster was too slow. Hope I do not lose points because of this.



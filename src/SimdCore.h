/*
 * SimdCore.h
 *
 *  Created on: Sep 23, 2015
 *      Author: harshitjain
 */

#ifndef SRC_SIMDCORE_H_
#define SRC_SIMDCORE_H_

#include <cstdint>
#include <map>

enum class ArgumentClass{AC_NONE, AC_2REG, AC_2IMM, AC_3REG, AC_3PREG, AC_3IMM, AC_3REGSRC, AC_1IMM,
	AC_1REG, AC_3IMMSRC, AC_PREGREG ,AC_2PREG};

template <typename T>
class SimdCore {
private:

	//In Bytes
	int instLength;
	//In Numbners
	int gprNum;
	int pregNum;
	int simdLanes;
	int warpNum;

	std::map<int,ArgumentClass> instrArgMap;


	//Inernal methods for execution
	unsigned int getGprNum(unsigned long long);
	unsigned int getPregNum(unsigned long long);
	unsigned int getOpcode(unsigned long long);
	bool checkPredicate(unsigned long long);
	bool checkPregRegister(unsigned long long);



	void instr_nop();																	// 0x00
	void instr_di();																	// 0x01
	void instr_ei();																	// 0x02
	void instr_tlbadd();	// 0x03
	void instr_tlbflush();
	void instr_neg();
	void instr_not();
	void instr_and();
	void instr_or();
	void instr_xor();
	void instr_add();
	void instr_sub();
	void instr_mul();
	void instr_div();
	void instr_mod();
	void instr_shl();
	void instr_shr();
	void instr_andi();
	void instr_ori();
	void instr_xori();
	void instr_addi();
	void instr_subi();
	void instr_muli();
	void instr_divi();
	void instr_modi();
	void instr_shli();
	void instr_shri();
	void instr_jali();
	void instr_jalr();
	void instr_jmpi();
	void instr_jmpr();
	void instr_clone();
	void instr_jalis();
	void instr_jalrs();
	void instr_jmprt();
	void instr_ld();
	void instr_st();
	void instr_ldi();
	void instr_rtop();
	void instr_andp();
	void instr_orp();
	void instr_xorp();
	void instr_notp();
	void instr_isneg();
	void instr_iszero();
	void instr_halt();
	void instr_trap();
	void instr_jmpru();
	void instr_skep();
	void instr_reti();
	void instr_tlbrm();
	void instr_itof();
	void instr_ftoi();
	void instr_fadd();
	void instr_fsub();
	void instr_fmul();
	void instr_fdiv();
	void instr_fneg();
	void instr_wspawn();
	void instr_split();
	void instr_join();
	void instr_bar();






public:
	SimdCore(int, int, int, int, int);

	virtual ~SimdCore();
};

#endif /* SRC_SIMDCORE_H_ */

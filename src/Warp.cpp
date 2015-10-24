/*
 * Warp.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: harshitjain
 */

#include "Warp.h"
#include "harp_emulator_driver.h"

template <typename T>
Warp<T>::Warp() {
    warpSize = DEFAULT_WN;
    currMask = new bool[warpSize];
    for(uint64_t i =0; i<warpSize;i++){
        currMask[i] = true;
    }
}

template <typename T>
Warp<T>::Warp(uint64_t _warpSize){
    warpSize = _warpSize;
    currMask = new bool[warpSize];
    for(uint64_t i =0; i<warpSize;i++){
        currMask[i] = true;
    }
}

template <typename T>
void Warp<T>::reconvStackPush(reconvStackElem<T> elem){
    reconvStack.push(elem);
}

template <typename T>
void Warp<T>::reconvStackPop(){
    reconvStack.pop();
}

template <typename T>
reconvStackElem<T> Warp<T>::reconvStackTop(){
    return reconvStack.top();
}






template <typename T>
bool Warp<T>::reconvStackEmpty(){
    return reconvStack.empty();
}





template <typename T>
Warp<T>::~Warp() {
	// TODO Auto-generated destructor stub
}


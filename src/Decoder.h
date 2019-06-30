/*
 * Decoder.h
 *
 *  Created on: Jun 27, 2019
 *      Author: bigm
 */

#ifndef DECODER_H_
#define DECODER_H_
#include "systemc.h"
#include <vector>

#define parityNUM 2



SC_MODULE (decoder){
	sc_in_clk clock;
	sc_in<sc_bit> in[parityNUM];
	sc_out<sc_bit> out[parityNUM];
	//number of states
	std::vector<viterbiPath> data;


	int numStates = 2>>parityNUM;
	int iterations = 2*parityNUM +1;
	//int i=0;
	void calculateBranchHammingDist(sc_bit receivedCode[2],sc_bit testCode[2]){

	}


	SC_CTOR(decoder){

	}





};


SC_MODULE(trellis){


};






#endif /* DECODER_H_ */

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

#define S_A 0b00
#define S_B 0b01
#define S_C 0b10
#define S_D 0b11

#define parityNUM 2
struct {
	bool currstate[2];
	bool nextstate[2];
	int branchHammingDistance = 0;
}typedef viterbiBranch;

struct {
	int pathHammingDist;
	std::vector<viterbiBranch> branchList;
	std::vector<bool> mLikelyStream;
}typedef viterbiPath;

struct {
	int lstState1;
	int lstState2;
	int winningPrvState;
}typedef node;


SC_MODULE (decoder){
	sc_in_clk clock;

	sc_in<sc_bit> in[5*2];
	sc_in<sc_uint<4>> outputBuffSize;
	sc_out<sc_bit> out[5*2];

	node trellis[5][4];

	//number of states
	std::vector<viterbiPath> pathList;


	int numStates = 2>>parityNUM;
	int iterations = 2*parityNUM +1;
	//int i=0;
	void calculateBranchHammingDist(sc_bit receivedCode[2],sc_bit testCode[2]){

	}
	void decoderProcess(){

		//pathList.at(pathList.size()- 1);
		int loopCounter = 0;
		while (loopCounter<5){
			//..const sc_core::sc_in::data_type bit1;
			//const sc_core::sc_in::data_type bit1;
			//sc_core::sc_in<sc_dt::sc_bit> bit1,bit2;
			//sc_bit bit1,bit2;
			int bit1 = in[loopCounter].read();
			int bit2 = in[loopCounter+1].read();
			int combinedInput = bit1 & (bit2<<1);

			//const sc_core::sc_in::data_type bit2;
			//sc_core::sc_in<sc_dt::sc_bit> bit2;
			//int bit2 = in[loopCounter+1].read();
		}
	}

	void createStateTable(){
		for (int i=0;i<5;i++){
			//initialize trellis nodes
			//initialize first node in the column
			trellis[i][0].lstState1 = S_A;
			trellis[i][0].lstState2 = S_B;
			//initialize second node
			trellis[i][1].lstState1 = S_C;
			trellis[i][1].lstState2 = S_D;
			//intialize third node
			trellis[i][2].lstState1 = S_A;
			trellis[i][2].lstState2 = S_B;
			//intialize fourth node
			trellis[i][2].lstState1 = S_C;
			trellis[i][2].lstState2 = S_D;
		}
	}
	SC_CTOR(decoder){

	}





};





SC_MODULE(trellis){


};






#endif /* DECODER_H_ */

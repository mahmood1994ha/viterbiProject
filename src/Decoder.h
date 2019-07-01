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
#define OUT_BUF_SIZE 5
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
	int nodeHammingDist;
}typedef node;


SC_MODULE (decoder){
	sc_in_clk clock;

	sc_in<sc_bit> in[OUT_BUF_SIZE*2];
	sc_out<sc_bit> out[OUT_BUF_SIZE*2];

	node trellis[5][4];

	//number of states
	std::vector<viterbiPath> pathList;


	int numStates = 2>>parityNUM;
	int iterations = 2*parityNUM +1;
	//int i=0;
	void calculateBranchHammingDist(sc_bit receivedCode[2],sc_bit testCode[2]){

	}
	void decoderProcess(){
		int loopCounter = 0;
		while (loopCounter<5){
			int bit1 = in[loopCounter].read();
			int bit2 = in[loopCounter+1].read();
			int combinedInput = bit1 & (bit2<<1);
			for (int i=0;i<4;i++){
				if (loopCounter==0){
					int firstCmpr = trellis[loopCounter][i].lstState1 ^ combinedInput;
					int secondCmpr = trellis[loopCounter][i].lstState2 ^ combinedInput;
					if (firstCmpr>secondCmpr){
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
						trellis[loopCounter][i].nodeHammingDist = firstCmpr;
					}else {
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
						trellis[loopCounter][i].nodeHammingDist = secondCmpr;
					}

				} else {
					int firstCmpr = trellis[loopCounter][i].lstState1 ^ combinedInput;
					int secondCmpr = trellis[loopCounter][i].lstState2 ^ combinedInput;
					if (firstCmpr>secondCmpr){
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
						int accumulatedPrvWeight = 0;
						if (trellis[loopCounter][i].lstState1==S_A){
							accumulatedPrvWeight = trellis[loopCounter-1][0].nodeHammingDist;
						}else {
							accumulatedPrvWeight = trellis[loopCounter-1][2].nodeHammingDist;
						}
						trellis[loopCounter][i].nodeHammingDist = firstCmpr + accumulatedPrvWeight;
					}else {
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
						int accumulatedPrvWeight = 0;
						if (trellis[loopCounter][i].lstState1==S_B){
							accumulatedPrvWeight = trellis[loopCounter-1][1].nodeHammingDist;
						}else {
							accumulatedPrvWeight = trellis[loopCounter-1][3].nodeHammingDist;
						}
						trellis[loopCounter][i].nodeHammingDist = secondCmpr + accumulatedPrvWeight;
					}

				}
			}
			loopCounter++;
		}
	}

	void createStateTable(){
		for (int i=0;i<5;i++){
			//initialize trellis nodes
			//initialize first node in the column
			trellis[i][0].lstState1 = S_A;
			trellis[i][0].lstState2 = S_B;
			trellis[i][0].nodeHammingDist = 0;
			//initialize second node
			trellis[i][1].lstState1 = S_C;
			trellis[i][1].lstState2 = S_D;
			trellis[i][0].nodeHammingDist = 0;
			//intialize third node
			trellis[i][2].lstState1 = S_A;
			trellis[i][2].lstState2 = S_B;
			trellis[i][0].nodeHammingDist = 0;
			//intialize fourth node
			trellis[i][2].lstState1 = S_C;
			trellis[i][2].lstState2 = S_D;
			trellis[i][0].nodeHammingDist = 0;
		}
	}
	SC_CTOR(decoder){

	}





};





SC_MODULE(trellis){


};






#endif /* DECODER_H_ */

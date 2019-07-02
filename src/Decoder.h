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

	node trellis[OUT_BUF_SIZE][4];
	sc_bit inBuffer[2*OUT_BUF_SIZE] = {sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('0'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('0'),
			sc_bit('1'),
	};
	//number of states
	std::vector<viterbiPath> pathList;


	int numStates = 2>>parityNUM;
	int iterations = 2*parityNUM +1;
	//int i=0;
	void calculateBranchHammingDist(sc_bit receivedCode[2],sc_bit testCode[2]){
	}
	void readInput(){
		for (int i=0;i<OUT_BUF_SIZE*2;i++){
			inBuffer[i] = in[i].read();
		}
	}
	void decoderProcess(){
		int loopCounter = 0;
		while (loopCounter<OUT_BUF_SIZE){
			int bit1 = inBuffer[loopCounter*2];
			cout<<"bit1:"<< bit1;
			int bit2 = inBuffer[loopCounter*2+1];
			cout<<",bit2:"<<bit2;
			int combinedInput = bit1;
			combinedInput = (combinedInput<<1) | bit2;
			cout<<",combined:"<<combinedInput<<"\n"<<endl;
			//cout<<"input"
			for (int i=0;i<4;i++){
				if (loopCounter==0){
					//continue;
					//int firstCmpr = trellis[loopCounter][i].lstState1 - combinedInput;
					int firstCmpr = get_metrics(trellis[loopCounter][i].lstState1,combinedInput);
					//cout<<"firstCmpr"<<firstCmpr<<"\n"<<endl;
					//int secondCmpr = trellis[loopCounter][i].lstState2 - combinedInput;
					int secondCmpr = get_metrics(trellis[loopCounter][i].lstState2,combinedInput);
					//cout<<"secondCmpr"<<secondCmpr<<"\n"<<endl;
					cout<<"====================================\n"<<endl;

					if (firstCmpr>secondCmpr){
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
						trellis[loopCounter][i].nodeHammingDist = 0;
					}else {
						trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
						trellis[loopCounter][i].nodeHammingDist = 0;
					}
				} else {
					int firstCmpr = get_metrics(trellis[loopCounter][i].lstState1,combinedInput);
					//= trellis[loopCounter][i].lstState1 ^ combinedInput;
					int secondCmpr = get_metrics(trellis[loopCounter][i].lstState2,combinedInput);
					//= trellis[loopCounter][i].lstState2 ^ combinedInput;
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
				cout<<"trellis row "<< i<<" ,col"<< loopCounter<<" ,accum hamming dist"<<trellis[loopCounter][i].nodeHammingDist<<"\n"<<endl;
			}
			loopCounter = loopCounter+1;
		}

		int minHammingDist = 99;
		int mLikelyPathState = -1;
		for (int i=0;i<4;i++){
			int temp = trellis[OUT_BUF_SIZE-1][i].nodeHammingDist;
			cout<<"state "<<i<<" hammingDist = "<<temp<<"\n"<<endl;
			if (temp<minHammingDist){
				minHammingDist = temp;
				mLikelyPathState = i;
			}
		}
		int stateIndex = mLikelyPathState;
		cout<<"Most likely state at EOT "<<stateIndex<<"\n"<<endl;
		cout<<"least accumulated metric "<<minHammingDist <<"\n"<<endl;
		traceMostLikelyPath(stateIndex);
	}
	int getSplitBits(int combinedInput,int bitNum){
		int retVal;
		if (bitNum==0){
			retVal = combinedInput&1;
		}else if (bitNum==1){
			retVal = combinedInput&0b10;
		}
		return retVal;
	}
	void traceMostLikelyPath(int stateIndex){
		int stateToTrack = stateIndex;
		int loopCounter = OUT_BUF_SIZE;
		while (loopCounter!=0){
			int stateToBackTrack = trellis[loopCounter-1][stateToTrack].winningPrvState;
			if(stateToBackTrack == S_A){
				stateToTrack = 0;
			}else if (stateToBackTrack == S_B){
				stateToTrack = 1;
			}else if (stateToBackTrack == S_C){
				stateToTrack = 2;
			}else if (stateToBackTrack == S_D){
				stateToTrack = 3;
			}
			cout <<"state "<<stateToBackTrack<<"\n"<<endl;
			loopCounter--;
		}
	}
	uint get_metrics(uint val_0, uint val_1) {
		uint distance = 0;
		val_0 ^= val_1;
		while (val_0) {

			if (val_0 & 0x1) {
				distance++;
			}
			val_0 = val_0 >> 1;
		}
		return distance;
	}
	void createStateTable(){
		cout<<"S1"<<"   "<<"S2"<<"   "<<"S3"<<"   "<<"S4"<<"\n"<<endl;
		for (int i=0;i<OUT_BUF_SIZE;i++){
			//initialize trellis nodes
			//initialize first node in the column
			trellis[i][0].lstState1 = S_A;
			trellis[i][0].lstState2 = S_B;
			trellis[i][0].nodeHammingDist = 0;
			//initialize second node
			trellis[i][1].lstState1 = S_C;
			trellis[i][1].lstState2 = S_D;
			trellis[i][1].nodeHammingDist = 0;
			//intialize third node
			trellis[i][2].lstState1 = S_A;
			trellis[i][2].lstState2 = S_B;
			trellis[i][2].nodeHammingDist = 0;
			//intialize fourth node
			trellis[i][3].lstState1 = S_C;
			trellis[i][3].lstState2 = S_D;
			trellis[i][3].nodeHammingDist = 0;
			cout<<trellis[i][0].lstState1<<"   "<<trellis[i][1].lstState1<<"   "<<trellis[i][2].lstState1<<"   "<<trellis[i][3].lstState1<<"\n"<<endl;
			cout<<trellis[i][0].lstState2<<"   "<<trellis[i][1].lstState2<<"   "<<trellis[i][2].lstState2<<"   "<<trellis[i][3].lstState2<<"\n"<<endl;
			cout<<trellis[i][0].nodeHammingDist<<"   "<<trellis[i][1].nodeHammingDist<<"   "<<trellis[i][2].nodeHammingDist<<"   "<<trellis[i][3].nodeHammingDist<<"\n"<<endl;
			cout<<"------------------------------\n"<<endl;
		}
	}
	void decoderIntegration(){
		//readInput();
		//decoderProcess();
	}
	SC_CTOR(decoder){
		cout<<"creating state table\n"<<endl;
		createStateTable();
		SC_METHOD(decoderIntegration);
		sensitive << clock.pos();
		SC_METHOD(decoderProcess);
	}
};
SC_MODULE(trellis){


};
#endif /* DECODER_H_ */

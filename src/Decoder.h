/*
 * Decoder.h
 *
 *  Created on: Jun 27, 2019
 *      Author: bigm
 */

#ifndef DECODER_H_
#define DECODER_H_
#include "systemc.h"
#include <bitset>
#include <vector>

#define S_A 0b00
#define S_B 0b01
#define S_C 0b10
#define S_D 0b11

#define parityNUM 2
#define OUT_BUF_SIZE 7
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
	int stateID;
	int prv_state1;
	int prv_state2;
	int prv_out1;
	int prv_out2;
	int prv_bit1;
	int prv_bit2;
	//winning path info
	int winning_prv_state;
	int winning_prv_out;
	int winning_prv_bit;
	//accumulated hamming distance
	int nodeHammingDist;
}typedef node;


SC_MODULE (decoder){
	sc_in_clk clock;

	sc_in<sc_bit> in[OUT_BUF_SIZE*2];
	sc_out<sc_bit> out[OUT_BUF_SIZE*2];

	node trellis[OUT_BUF_SIZE][4];
	sc_bit inBuffer[2*OUT_BUF_SIZE] = {
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('0'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('0'),
			sc_bit('1'),
			sc_bit('0'),
			sc_bit('1'),
			sc_bit('1'),
			sc_bit('1')
	};
	//number of states
	int inBufferCombined[OUT_BUF_SIZE];
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
			if (loopCounter==0){
				loopCounter++;
				continue;
			}
			int combinedInput = inBufferCombined[loopCounter-1];
			cout<<",combined:"<<std::bitset<2>(combinedInput)<<"\n"<<endl;
			//cout<<"input"
			int i=0;
			for (i=0;i<4;i++){
				int firstIncomingBranch = get_metrics(trellis[loopCounter][i].prv_out1,combinedInput);
				int secondIncomingBranch = get_metrics(trellis[loopCounter][i].prv_out2,combinedInput);

				cout<<"=======================================\n"<<endl;
				cout<<"first incoming path cost "<< firstIncomingBranch<<"\n"<<endl;
				cout<<"second incoming path cost "<< secondIncomingBranch<<"\n"<<endl;
				cout<<"=======================================\n"<<endl;
				int prvHammingDist1=0;
				int prvHammingDist2=0;
				//if (loopCounter>0){
					prvHammingDist1 = trellis[loopCounter-1][trellis[loopCounter][i].prv_state1].nodeHammingDist;
					prvHammingDist2 = trellis[loopCounter-1][trellis[loopCounter][i].prv_state2].nodeHammingDist;
					cout<<"prvHD1:"<<prvHammingDist1<<" ,prvHD2:"<<prvHammingDist2<<"\n"<<endl;
				//}
				firstIncomingBranch +=prvHammingDist1;
				secondIncomingBranch +=prvHammingDist2;
				if (firstIncomingBranch<secondIncomingBranch){
					cout<<"first branch chosen\n"<<endl;
					//if the first branch has smaller hamming distance
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state1;
					trellis[loopCounter][i].winning_prv_out = trellis[loopCounter][i].prv_out1;
					trellis[loopCounter][i].winning_prv_bit = trellis[loopCounter][i].prv_bit1;
					//find the previous hamming dist and add the new one to it
					trellis[loopCounter][i].nodeHammingDist =firstIncomingBranch;
				}
				else {
					cout<<"second branch chosen\n"<<endl;
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state2;
					trellis[loopCounter][i].winning_prv_out = trellis[loopCounter][i].prv_out2;
					trellis[loopCounter][i].winning_prv_bit = trellis[loopCounter][i].prv_bit2;
					//find the previous hamming dist and add the new one to it
					trellis[loopCounter][i].nodeHammingDist = secondIncomingBranch;
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
		cout<<"least accumulated metric "<<minHammingDist <<"\n"<<endl;
		cout<<"Most likely path at EOT: \n"<<endl;
		cout<<"state "<<std::bitset<2>(stateIndex)<<"\n"<<endl;
		traceMostLikelyPath(stateIndex);
	}



	void decoderProcess2(){
		int loopCounter=1;
		while (loopCounter<OUT_BUF_SIZE){
			for (int i=0;i<4;i++){
				int currInput = inBufferCombined[loopCounter-1];
				int branchMetric1 = get_metrics(trellis[loopCounter][i].prv_out1,currInput);
				int branchMetric2 = get_metrics(trellis[loopCounter][i].prv_out2,currInput);
				int oldVal = 0;
				if (loopCounter>0){
					if (trellis[loopCounter][i].stateID==0){
						int x1 = trellis[loopCounter-1][0].nodeHammingDist;
						int x2 = trellis[loopCounter-1][1].nodeHammingDist;
						if (x1<x2){
							oldVal = x1;
						}else {
							oldVal = x2;
						}
					}else if (trellis[loopCounter][i].stateID==1){
						int x1 = trellis[loopCounter-1][2].nodeHammingDist;
						int x2 = trellis[loopCounter-1][3].nodeHammingDist;
						if (x1<x2){
							oldVal = x1;
						}else {
							oldVal = x2;
						}
					}else if (trellis[loopCounter][i].stateID==2){
						int x1 = trellis[loopCounter-1][0].nodeHammingDist;
						int x2 = trellis[loopCounter-1][1].nodeHammingDist;
						if (x1<x2){
							oldVal = x1;
						}else {
							oldVal = x2;
						}
					}else {
						int x1 = trellis[loopCounter-1][2].nodeHammingDist;
						int x2 = trellis[loopCounter-1][3].nodeHammingDist;
						if (x1<x2){
							oldVal = x1;
						}else {
							oldVal = x2;
						}
					}
				}
				if (branchMetric1<branchMetric2){
					//declare the winning branch
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state1;
					trellis[loopCounter][i].nodeHammingDist = branchMetric1 + oldVal;
				}else {
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state1;
					trellis[loopCounter][i].nodeHammingDist = branchMetric2 + oldVal;
				}

			}
			loopCounter++;
		}
		int minHammingDistance = 100;
		int mstLikelyState = -1;
		for (int i=0;i<4;i++){
			int temp = trellis[OUT_BUF_SIZE - 1][i].nodeHammingDist;
			if (temp<minHammingDistance){
				minHammingDistance = temp;
				mstLikelyState = trellis[OUT_BUF_SIZE - 1][i].stateID;
			}
		}
		traceMostLikelyPath(mstLikelyState);
	}
	void traceBack(int EOT_state){
		int loopCounter ;
		int stateBackTrack = EOT_state;
		for (loopCounter= 0;loopCounter<OUT_BUF_SIZE;loopCounter++){
			int oldVal = 0;
			if (loopCounter==0){
				stateBackTrack = 0;
				int x1 = trellis[loopCounter+1][0].nodeHammingDist;
				int x2 = trellis[loopCounter+1][1].nodeHammingDist;
				if (x1<x2){
					cout<<trellis[loopCounter+1][0].stateID;
					oldVal = x1;
				}else {
					cout<<trellis[loopCounter+1][1].stateID;
					oldVal = x2;
				}
			}
			if (loopCounter>0){
				if (stateBackTrack==0){
					int x1 = trellis[loopCounter+1][0].nodeHammingDist;
					int x2 = trellis[loopCounter+1][1].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter+1][0].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter+1][1].stateID;
						oldVal = x2;
					}
				}else if (stateBackTrack==1){
					int x1 = trellis[loopCounter+1][2].nodeHammingDist;
					int x2 = trellis[loopCounter+1][3].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter+1][2].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter+1][3].stateID;
						oldVal = x2;
					}
				}else if (stateBackTrack){
					int x1 = trellis[loopCounter+1][0].nodeHammingDist;
					int x2 = trellis[loopCounter+1][1].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter+1][0].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter+1][1].stateID;
						oldVal = x2;
					}
				}else {
					int x1 = trellis[loopCounter+1][2].nodeHammingDist;
					int x2 = trellis[loopCounter+1][3].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter+1][2].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter+1][2].stateID;
						oldVal = x2;
					}
				}
				cout<<"\n"<<endl;
			}
		}
	}
	/*void traceForward(int EOT){
		for (int i=0;i<OUT_BUF_SIZE;i++){
			int temp
			for (int j=0;i<4;j++){


			}
		}
	}*/
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
		int loopCounter = OUT_BUF_SIZE-1;
		cout <<"likely input "<<trellis[loopCounter][stateToTrack].winning_prv_bit<<"\n"<<endl;
		while (loopCounter>0){
			stateToTrack = trellis[loopCounter][stateToTrack].winning_prv_state;
			cout <<"state "<<std::bitset<2>(stateToTrack)<<"\n"<<endl;
			cout <<"likely input "<<trellis[loopCounter][stateToTrack].winning_prv_bit<<"\n"<<endl;
			loopCounter--;
		}
	}
	uint get_metrics(int val1, int val2)
	{
		unsigned int ret_val = 0;
		unsigned int i = 0;

		std::bitset<sizeof(int)> val1_bits(val1);
		std::bitset<sizeof(int)> val2_bits(val2);

		for(i = 0; i < (sizeof(int)) ;i++)
		{
			((val1_bits[i] != val2_bits[i]) ? ret_val++ : 0);
		}

		return ret_val;
	}

	int combineBits(int bit1,int bit2){
		int combinedInput = bit1;
		combinedInput = (combinedInput<<1) | bit2;
		return combinedInput;
	}
	void createStateTable(){
		cout<<"S1"<<"   "<<"S2"<<"   "<<"S3"<<"   "<<"S4"<<"\n"<<endl;
		for (int i=0;i<OUT_BUF_SIZE;i++){
			//initialize trellis nodes
			//initialize first node in the column
			trellis[i][0].stateID = S_A;
			trellis[i][0].prv_state1=S_A;
			trellis[i][0].prv_state2=S_B;
			trellis[i][0].prv_out1 = 0b00;
			trellis[i][0].prv_out2 = 0b11;
			trellis[i][0].prv_bit1=0b00;
			trellis[i][0].prv_bit2=0b00;
			trellis[i][0].nodeHammingDist = 0;
			//initialize second node
			trellis[i][1].stateID = S_B;
			trellis[i][1].prv_state1 = S_C;
			trellis[i][1].prv_state2 = S_D;
			trellis[i][1].prv_out1 = 0b10;
			trellis[i][1].prv_out2 = 0b01;
			trellis[i][1].prv_bit1 = 0b00;
			trellis[i][1].prv_bit2 = 0b00;
			trellis[i][1].nodeHammingDist = 0;
			//intialize third node
			trellis[i][2].stateID = S_C;
			trellis[i][2].prv_state1 = S_A;
			trellis[i][2].prv_state2 = S_B;
			trellis[i][2].prv_out1 = 0b11;
			trellis[i][2].prv_out2 = 0b00;
			trellis[i][2].prv_bit1 = 0b01;
			trellis[i][2].prv_bit2 = 0b01;
			trellis[i][2].nodeHammingDist = 0;
			//initialize last node
			trellis[i][3].stateID = S_D;
			trellis[i][3].prv_state1 = S_C;
			trellis[i][3].prv_state2 = S_D;
			trellis[i][3].prv_out1 = 0b01;
			trellis[i][3].prv_out2 = 0b10;
			trellis[i][3].prv_bit1 = 0b01;
			trellis[i][3].prv_bit2 = 0b01;
			trellis[i][3].nodeHammingDist = 0;
		}

		//memset
	}

	void joinInputArray(){
		int i=0;
		int j=0;
		while (i<2*OUT_BUF_SIZE){
			inBufferCombined[j] = combineBits(inBuffer[i],inBuffer[i+1]);
			i +=2;
			j++;
		}
	}
	void decoderIntegration(){
		//readInput();
		//decoderProcess();
	}
	SC_CTOR(decoder){
		cout<<"creating state table\n"<<endl;
		createStateTable();
		joinInputArray();
		SC_METHOD(decoderIntegration);
		sensitive << clock.pos();
		SC_METHOD(decoderProcess);
	}
};
SC_MODULE(trellis){


};
#endif /* DECODER_H_ */

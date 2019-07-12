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
			//cout<<",combined:"<<std::bitset<2>(combinedInput)<<"\n"<<endl;
			int i=0;
			for (i=0;i<4;i++){
				int firstIncomingBranch = get_hamming_distance(trellis[loopCounter][i].prv_out1,combinedInput);
				int secondIncomingBranch = get_hamming_distance(trellis[loopCounter][i].prv_out2,combinedInput);
/*				cout<<"=======================================\n"<<endl;
				cout<<"first incoming path cost "<< firstIncomingBranch<<"\n"<<endl;
				cout<<"second incoming path cost "<< secondIncomingBranch<<"\n"<<endl;
				cout<<"=======================================\n"<<endl;*/
				int prvHammingDist1=0;
				int prvHammingDist2=0;
				prvHammingDist1 = trellis[loopCounter-1][trellis[loopCounter][i].prv_state1].nodeHammingDist;
				prvHammingDist2 = trellis[loopCounter-1][trellis[loopCounter][i].prv_state2].nodeHammingDist;
				//cout<<"prvHD1:"<<prvHammingDist1<<" ,prvHD2:"<<prvHammingDist2<<"\n"<<endl;
				firstIncomingBranch +=prvHammingDist1;
				secondIncomingBranch +=prvHammingDist2;
				if (firstIncomingBranch<secondIncomingBranch){
					//cout<<"first branch chosen\n"<<endl;
					//if the first branch has smaller hamming distance
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state1;
					trellis[loopCounter][i].winning_prv_out = trellis[loopCounter][i].prv_out1;
					trellis[loopCounter][i].winning_prv_bit = trellis[loopCounter][i].prv_bit1;
					//find the previous hamming dist and add the new one to it
					trellis[loopCounter][i].nodeHammingDist =firstIncomingBranch;
				}
				else {
					//cout<<"second branch chosen\n"<<endl;
					trellis[loopCounter][i].winning_prv_state = trellis[loopCounter][i].prv_state2;
					trellis[loopCounter][i].winning_prv_out = trellis[loopCounter][i].prv_out2;
					trellis[loopCounter][i].winning_prv_bit = trellis[loopCounter][i].prv_bit2;
					//find the previous hamming dist and add the new one to it
					trellis[loopCounter][i].nodeHammingDist = secondIncomingBranch;
				}
				//cout<<"trellis row "<< i<<" ,col"<< loopCounter<<" ,accum hamming dist"<<trellis[loopCounter][i].nodeHammingDist<<"\n"<<endl;
			}

			loopCounter = loopCounter+1;
		}
		int minHammingDist = 99;
		int mLikelyPathState = -1;
		cout<<"metrics at EOT\n"<<endl;
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
		cout<<"Most likely path at EOT (reversed): \n"<<endl;
		cout<<"state "<<std::bitset<2>(stateIndex)<<"\n"<<endl;
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
		int loopCounter = OUT_BUF_SIZE-1;
		cout <<"likely input "<<trellis[loopCounter][stateToTrack].winning_prv_bit<<"\n"<<endl;
		while (loopCounter>0){
			stateToTrack = trellis[loopCounter][stateToTrack].winning_prv_state;
			cout <<"state "<<std::bitset<2>(stateToTrack)<<"\n"<<endl;
			cout <<"likely input "<<trellis[loopCounter][stateToTrack].winning_prv_bit<<"\n"<<endl;
			loopCounter--;
		}
	}
	uint get_hamming_distance(int val1, int val2)
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
		cout<<"creating state table....\n"<<endl;
		createStateTable();
		cout<<"state table created\n"<<endl;
		joinInputArray();
		SC_METHOD(decoderIntegration);
		sensitive << clock.pos();
		SC_METHOD(decoderProcess);
	}
};
SC_MODULE(trellis){


};
#endif /* DECODER_H_ */

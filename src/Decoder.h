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

#define S_A 0
#define S_B 1
#define S_C 2
#define S_D 3

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
			/*int bit1 = inBuffer[loopCounter];
			cout<<"bit1:"<< bit1;
			int bit2 = inBuffer[loopCounter*2+1];
			cout<<",bit2:"<<bit2;
			int combinedInput = bit1;
			combinedInput = (combinedInput<<1) | bit2;*/
			int combinedInput = inBufferCombined[loopCounter-1];
			cout<<",combined:"<<combinedInput<<"\n"<<endl;
			//cout<<"input"
			int i=0;
			for (i=0;i<4;i++){
				int firstIncomingBranch = get_metrics(trellis[loopCounter][i].lstState1,combinedInput);
				int secondIncomingBranch = get_metrics(trellis[loopCounter][i].lstState2,combinedInput);
				/*cout<<"=======================================\n"<<endl;
					cout<<"first incoming path cost "<< firstIncomingBranch<<"\n"<<endl;
					cout<<"second incoming path cost "<< secondIncomingBranch<<"\n"<<endl;
					cout<<"=======================================\n"<<endl;*/
				if (firstIncomingBranch==0){
					cout<<"first branch chosen\n"<<endl;
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
					//find the previous hamming dist and add the new one to it
					int prvHammingDist = 0;
					if (loopCounter>0){
						prvHammingDist = trellis[loopCounter-1][trellis[loopCounter][i].lstState1].nodeHammingDist;
					}else {
						continue;
					}
					trellis[loopCounter][i].nodeHammingDist = prvHammingDist + firstIncomingBranch;
					//continue;
				}else if (secondIncomingBranch==0){
					cout<<"second branch chosen\n"<<endl;
					//do the same for if the other branch is more likely
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
					//find the previous hamming dist and add the new one to it
					int prvHammingDist = 0;
					if (loopCounter>0){
						prvHammingDist = trellis[loopCounter-1][trellis[loopCounter][i].lstState2].nodeHammingDist;
					} else {
						continue;
					}
					trellis[loopCounter][i].nodeHammingDist = prvHammingDist + secondIncomingBranch;
					//continue;
				} else if (firstIncomingBranch<secondIncomingBranch){
					cout<<"first branch chosen\n"<<endl;
					//if the first branch has smaller hamming distance
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
					//find the previous hamming dist and add the new one to it
					int prvHammingDist = 0;
					if (loopCounter>0){
						prvHammingDist = trellis[loopCounter-1][trellis[loopCounter][i].lstState1].nodeHammingDist;
					} else{
						continue;
					}
					trellis[loopCounter][i].nodeHammingDist = prvHammingDist + firstIncomingBranch;
					//continue;
				}
				else {
					cout<<"second branch chosen\n"<<endl;
					//do the same for if the other branch is more likely
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
					//find the previous hamming dist and add the new one to it
					int prvHammingDist = 0;
					if (loopCounter>0){
						prvHammingDist = trellis[loopCounter-1][trellis[loopCounter][i].lstState2].nodeHammingDist;
					}else {
						continue;
					}
					trellis[loopCounter][i].nodeHammingDist = prvHammingDist + secondIncomingBranch;
					//continue;
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




	void decoderProcess2(){
		int loopCounter=0;
		while (loopCounter<OUT_BUF_SIZE){
			for (int i=0;i<4;i++){
				int currInput = inBufferCombined[loopCounter];
				int branchMetric1 = get_metrics(trellis[loopCounter][i].lstState1,currInput);
				int branchMetric2 = get_metrics(trellis[loopCounter][i].lstState2,currInput);
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
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState1;
					trellis[loopCounter][i].nodeHammingDist = branchMetric1 + oldVal;
				}else {
					trellis[loopCounter][i].winningPrvState = trellis[loopCounter][i].lstState2;
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
		traceBack(mstLikelyState);
	}
	void traceBack(int EOT_state){
		int loopCounter ;
		int stateBackTrack = EOT_state;
		for (loopCounter= OUT_BUF_SIZE - 1;loopCounter>=0;loopCounter--){
			int oldVal = 0;
			if (loopCounter>0){
				if (stateBackTrack==0){
					int x1 = trellis[loopCounter-1][0].nodeHammingDist;
					int x2 = trellis[loopCounter-1][1].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter-1][0].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter-1][1].stateID;
						oldVal = x2;
					}
				}else if (stateBackTrack==1){
					int x1 = trellis[loopCounter-1][2].nodeHammingDist;
					int x2 = trellis[loopCounter-1][3].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter-1][2].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter-1][3].stateID;
						oldVal = x2;
					}
				}else if (stateBackTrack){
					int x1 = trellis[loopCounter-1][0].nodeHammingDist;
					int x2 = trellis[loopCounter-1][1].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter-1][0].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter-1][1].stateID;
						oldVal = x2;
					}
				}else {
					int x1 = trellis[loopCounter-1][2].nodeHammingDist;
					int x2 = trellis[loopCounter-1][3].nodeHammingDist;
					if (x1<x2){
						cout<<trellis[loopCounter-1][2].stateID;
						oldVal = x1;
					}else {
						cout<<trellis[loopCounter-1][2].stateID;
						oldVal = x2;
					}
				}
				cout<<"\n"<<endl;
			}
		}
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
		while (loopCounter!=0){
			stateToTrack = trellis[loopCounter-1][stateToTrack].winningPrvState;
			cout <<"state "<<stateToTrack<<"\n"<<endl;
			loopCounter--;
		}
	}
	uint get_metrics(unsigned int l_value1_i, unsigned int l_value2_i)
	{
		unsigned int l_returnValue_i = 0;
		unsigned int i = 0;

		std::bitset<sizeof(unsigned int)> l_value1_bs(l_value1_i);
		std::bitset<sizeof(unsigned int)> l_value2_bs(l_value2_i);

		for(i = 0; i < (sizeof(unsigned int)) ;i++)
		{
			((l_value1_bs[i] != l_value2_bs[i]) ? l_returnValue_i++ : 0);
		}

		return l_returnValue_i;
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
			trellis[i][0].lstState1 = S_A;
			trellis[i][0].lstState2 = S_C;
			trellis[i][0].nodeHammingDist = 0;
			//initialize second node
			trellis[i][1].stateID = S_B;
			trellis[i][1].lstState1 = S_A;
			trellis[i][1].lstState2 = S_C;
			trellis[i][1].nodeHammingDist = 0;
			//intialize third node
			trellis[i][2].stateID = S_C;
			trellis[i][2].lstState1 = S_B;
			trellis[i][2].lstState2 = S_D;
			trellis[i][2].nodeHammingDist = 0;
			//intialize fourth node
			trellis[i][3].stateID = S_D;
			trellis[i][3].lstState1 = S_B;
			trellis[i][3].lstState2 = S_D;
			trellis[i][3].nodeHammingDist = 0;
			/*cout<<trellis[i][0].lstState1<<"   "<<trellis[i][1].lstState1<<"   "<<trellis[i][2].lstState1<<"   "<<trellis[i][3].lstState1<<"\n"<<endl;
			cout<<trellis[i][0].lstState2<<"   "<<trellis[i][1].lstState2<<"   "<<trellis[i][2].lstState2<<"   "<<trellis[i][3].lstState2<<"\n"<<endl;
			cout<<trellis[i][0].nodeHammingDist<<"   "<<trellis[i][1].nodeHammingDist<<"   "<<trellis[i][2].nodeHammingDist<<"   "<<trellis[i][3].nodeHammingDist<<"\n"<<endl;
			cout<<"------------------------------\n"<<endl;*/
		}
	}

	void joinInputArray(){
		int i=0;
		int j=0;
		while (i<2*OUT_BUF_SIZE){
			inBufferCombined[j] = combineBits(inBuffer[i],inBuffer[i+1]);
			//cout<<"joined input"<<inBufferCombined[i]<<"\n"<<endl;
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

/*
 * DecTest.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: ravithej
 */
#include "systemc.h"
#include "DecTest.h"

void dectest::source(){
	sc_bit tmp[10] = {"1011000110"};
	//1011000110
	/*
	tmp[0] = 0b00;
	tmp[1] = 0b1;
	tmp[2] = 0b1;
	tmp[3] = 0b0;
	tmp[4] = 0b0;
	tmp[5] = 0b0;
	tmp[6] = 0b1;
	tmp[7] = 0b1;
	tmp[8] = 0b0;
	tmp[9] = 0b1;*/

	for(int i = 0; i < 10; i++){
		in[i].write(tmp[i]);
		wait();
	}
}

void dectest::sink(){
	sc_in<sc_bit> inData[10];
	for(int i = 0; i < 10; i++){
		inData[i] = out[i].read();
		wait();
		cout << i << " :/t" << inData[i] << endl;
	}
	sc_stop();
}




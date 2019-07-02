/*
 * DecTest.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: ravithej
 */
#include "systemc.h"
#include "DecTest.h"

void dectest::source(){
	sc_signal<sc_bit> tmp[14];
	//1011000110

	tmp[0] = sc_bit('1');
	tmp[1] = sc_bit('1');
	tmp[2] = sc_bit('1');
	tmp[3] = sc_bit('0');
	tmp[4] = sc_bit('1');
	tmp[5] = sc_bit('1');
	tmp[6] = sc_bit('1');
	tmp[7] = sc_bit('1');
	tmp[8] = sc_bit('0');
	tmp[9] = sc_bit('1');
	tmp[10] = sc_bit('1');
	tmp[11] = sc_bit('1');
	tmp[12] = sc_bit('1');
	tmp[13] = sc_bit('1');

	for(int i = 0; i < 10; i++){
		in[i].write(tmp[i]);
		//wait();
	}
}

void dectest::sink(){
	sc_signal<sc_bit> inData[10];
	for(int i = 0; i < 10; i++){
		inData[i] = out[i].read();
		wait();
		cout << i << " :/t" << inData[i] << endl;
	}
	sc_stop();
}




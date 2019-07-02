/*
 * DecTest.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: ravithej
 */
#include "systemc.h"
#include "DecTest.h"

void dectest::source(){
	sc_signal<sc_bit> tmp[10];
	//1011000110
	tmp[0].write(sc_bit('1'));
	tmp[1].write(sc_bit('0'));
	tmp[2].write(sc_bit('1'));
	tmp[3].write(sc_bit('1'));
	tmp[4].write(sc_bit('0'));
	tmp[5].write(sc_bit('0'));
	tmp[6].write(sc_bit('0'));
	tmp[7].write(sc_bit('1'));
	tmp[8].write(sc_bit('1'));
	tmp[9].write(sc_bit('0'));

	for(int i = 0; i < 10; i++){
		in[i].write(tmp[i]);
		wait();
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




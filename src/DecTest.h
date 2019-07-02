/*
 * DecTest.h
 *
 *  Created on: Jul 1, 2019
 *      Author: ravithej
 */

#ifndef DECTEST_H_
#define DECTEST_H_

#include "systemc.h"

SC_MODULE(dectest){
	sc_in_clk clock;
	sc_out<sc_bit> in[14];
	sc_in<sc_bit> out[14];

	void source();
	void sink();

	SC_CTOR(dectest){
		SC_CTHREAD(source, clock.pos());
		SC_CTHREAD(sink, clock.pos());
	}

};





#endif /* DECTEST_H_ */

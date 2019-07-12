/*
 * DecTest.h
 *
 *  Created on: Jul 1, 2019
 *      Author: ravithej
 */

#ifndef DECTEST_H_
#define DECTEST_H_

#include "systemc.h"
#include "Decoder.h"

SC_MODULE(dectest){
	sc_in_clk clock;
	sc_out<sc_bit> in[14];
	sc_in<sc_bit> out[14];
	void source();
	void sink();
	void bind_concrete_decoder();

	SC_CTOR(dectest){
		//decoder h("hello");
		/*h.clock(clock);
		//bind_concrete_decoder();
		for (int i=0; i<14;i++){
			h.in[i](out[i]);
			h.out[i](in[i]);
		}*/
		//next_trigger();
		SC_CTHREAD(source, clock.pos());
		SC_CTHREAD(sink, clock.pos());
	}
};
#endif /* DECTEST_H_ */

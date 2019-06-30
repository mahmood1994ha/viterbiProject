/*
 * ShiftReg.h
 *
 *  Created on: Jun 27, 2019
 *      Author: bigm
 */

#ifndef SHIFTREG_H_
#define SHIFTREG_H_
#include "systemc.h"

SC_MODULE(shiftReg){
	 sc_in<bool> dataIn;/**< input bits to modulo adder */
	 sc_in<bool> clk;
	 sc_out<sc_uint<3>> dataOut;/**< output bit from modulo adder */

	 void shift(){
		 sc_uint<3> reg;
		 dataOut.write(0);
		 while(1){
			 wait();/**< wait for signal on sensitivity list*/
			 reg = dataOut;
			 reg[2] = reg[1];
			 reg[1] = reg[0];
			 reg[0] = dataIn;
			 wait(2, SC_NS); /**< delay to produce outputs.*/
			 dataOut.write(reg);
		 }
	 }

	 SC_CTOR(shiftReg)          // constructor for modulo Adder
	   {
	     SC_THREAD(shift);  // register modAdder with kernel
	     sensitive << clk.pos();  // sensitivity list
	   }
};



#endif /* SHIFTREG_H_ */

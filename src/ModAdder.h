/*
 * ModAdder.h
 *
 *  Created on: Jun 27, 2019
 *      Author: bigm
 */

#ifndef MODADDER_H_
#define MODADDER_H_
#include <systemc.h>

SC_MODULE(modAdder){
	 sc_in<sc_bit> in1, in2;/**< input bits to modulo adder */
	 sc_out<sc_bit> out1;/**< output bit from modulo adder */

	 void modAdd(){
		 out1.write( in1.read() ^ in2.read() );
	 }

	 SC_CTOR(modAdder)          // constructor for modulo Adder
	   {
	     SC_METHOD(modAdd);  // register modAdder with kernel
	     sensitive << in1 << in2;  // sensitivity list
	   }
};

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



#endif /* MODADDER_H_ */

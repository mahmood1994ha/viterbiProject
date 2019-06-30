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
#endif /* MODADDER_H_ */

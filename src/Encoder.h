#include "systemc.h"
SC_MODULE(Encoder)          // declare encoder module
{
	sc_in<sc_bit> input;
	sc_out<sc_bit> output1, output2;
	sc_bv<3> shiftregister;
	sc_in<sc_bit> count;
	sc_in<bool> Clk;

	void encode();

	SC_CTOR(Encoder)
	{
		
		shiftregister = "000";
		SC_THREAD(encode); //Executing the encode function
		sensitive << count;  //sensitive to reading input data
	}
};


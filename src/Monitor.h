#include "SystemC.h"
SC_MODULE(Monitor)          // declare encoder module
{
	sc_in<sc_bit> input;
	sc_out<sc_bit> output1, output2;
	sc_in<bool> Clk;
	sc_out<sc_bit> count;

	void monitor();

	SC_CTOR(Monitor)
	{
		
		SC_METHOD(monitor);
		sensitive << count;

	}
};
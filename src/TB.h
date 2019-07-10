
#include "systemc.h"

SC_MODULE(TB)
{
	sc_out<sc_bit> input;
	sc_in<bool> Clk;
	sc_out<sc_bit> count;

	void TBFunction();
	

	SC_CTOR(TB)
	{
		//cout << "In TB";
		SC_THREAD(TBFunction);
		sensitive << Clk.pos();
	}


};

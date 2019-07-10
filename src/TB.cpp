#include"TB.h"
#include"systemc.h"

void TB::TBFunction()
{
	
	// input is 1011
	input.write(sc_bit('1'));
	count.write(sc_bit('1'));
	wait(200, SC_NS);
	cout << "\n\n-------------------------------------------------- Output is:";
	input.write(sc_bit('0'));
	count.write(sc_bit('0'));
	wait(200, SC_NS);
	
	input.write(sc_bit('1'));
	count.write(sc_bit('1'));
	wait(200, SC_NS);
	
	input.write(sc_bit('1'));
	count.write(sc_bit('0'));
	wait(200, SC_NS);
	
	input.write(sc_bit('0'));
	count.write(sc_bit('1'));
	wait(200, SC_NS);
	
	input.write(sc_bit('0'));
	count.write(sc_bit('0'));
	wait(200, SC_NS);
	
	input.write(sc_bit('0'));
	count.write(sc_bit('1'));
	wait(200, SC_NS);
	
	count.write(sc_bit('0'));
	wait(200, SC_NS);
	//cout << "----------------------------------------------------------------------------";
	sc_stop();

}
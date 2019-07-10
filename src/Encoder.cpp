#include "Encoder.h"
#include "systemc.h"

void Encoder::encode()
{
	sc_bit out1, out2;
	
	//sc_bit shiftregister;

	while (1)

	{
		wait();
		for (int i = 3; i>0; i--)
		{
			shiftregister[i] = shiftregister[i - 1];
		}
		shiftregister[0] = input;
		wait(100, SC_NS);
		out1 = shiftregister[3] ^ shiftregister[2] ^ shiftregister[1] ^ shiftregister[0];
		out2 = shiftregister[3] ^ shiftregister[1] ^ shiftregister[0];
		output1.write(sc_bit(out1));
		output2.write(sc_bit(out2));
	}


}
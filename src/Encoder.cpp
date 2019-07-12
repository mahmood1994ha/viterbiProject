#include "Encoder.h"
#include "systemc.h"

void Encoder::encode()
{
	sc_bit out1, out2;

	while (1)

	{
		wait();
		for (int i = 3; i>0; i--)
		{
			shiftregister[i] = shiftregister[i - 1];

		}


		shiftregister[0] = input;

		cout <<"Input = "<<input<<"\t\t";
		wait(200, SC_NS);
		cout <<"Encoder Bit= ";
		for (int i = 0; i<4; i++)
		{
			cout<<shiftregister[i];

		}
		out1 = shiftregister[3]  ^shiftregister[2]  ^ shiftregister[1] ^ shiftregister[0];
		out2 = shiftregister[3]  ^shiftregister[1] ^ shiftregister[0];
		output1.write(sc_bit(out1));
		output2.write(sc_bit(out2));
		cout <<"\t\t Output ==>"<<out1<<out2<<"\t\t"<<"\n"<<endl;
	}


}

#include "systemc.h"
#include "Encoder.h"
#include "TB.h"
#include "Monitor.h"
#include "DecTest.h"
#include "Decoder.h"

int sc_main(int argc, char* argv[])
{
	sc_signal<sc_bit, SC_MANY_WRITERS> input, out1, out2,count;
	//bound decoder inputs
	sc_signal<sc_bit,SC_MANY_WRITERS> decoder_in[14];
	sc_signal<sc_bit,SC_MANY_WRITERS> decoder_out[14];
	sc_core::sc_report_handler::set_actions("/IEEE_Std_1666/deprecated",sc_core::SC_DO_NOTHING);

	sc_clock TestClk("TestClock", 10, SC_NS, 0.5);

	Encoder E("Encoder");
	E.input(input);
	E.output1(out1);
	E.output2(out2);
	E.count(count);
	E.Clk(TestClk);

	//decoder declaration
	decoder dect("decoder");
	dect.clock(TestClk);
	for (int i=0;i<14;i++){
		dect.in[i](decoder_in[i]);
	}
	for (int i=0;i<14;i++){
		dect.out[i](decoder_out[i]);
	}

	Monitor monitor("Monitor");
	monitor.input(input);
	monitor.output1(out1);
	monitor.output2(out2);
	monitor.Clk(TestClk);
	monitor.count(count);

	TB tb("TB");
	tb.input(input);
	tb.Clk(TestClk);
	tb.count(count);

	sc_start();

	return 0;

}

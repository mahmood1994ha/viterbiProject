// All systemc modules should include systemc.h header file
#include "systemc.h"
#include "tlm.h"

#include "Encoder.h"
#include "Decoder.h"

// Hello_world is module name
SC_MODULE (hello_world) {
  SC_CTOR (hello_world) {
    // Nothing in constructor
  }
  void say_hello() {
    //Print "Hello World" to the console.
    cout << "Hello World.\n";
  }
};

// sc_main in top level function like in C++ main
int sc_main(int argc, char* argv[]) {
	sc_signal<bool>   clock;
	  sc_signal<bool>   reset;
	  sc_signal<bool>   enable;
	  sc_signal<sc_uint<4> > counter_out;
	  sc_signal <sc_bit> decoderIn[10];
	  sc_signal <sc_bit> decoderOut[10];

	  int i = 0;
	  // Connect the DUT
	  first_counter counter("COUNTER");
	    counter.clock(clock);
	    counter.reset(reset);
	    counter.enable(enable);
	    counter.counter_out(counter_out);
	  //declare decoder
	  //decoder
	  decoderIn[0].write(sc_bit('0'));
	  sc_start(1,SC_NS,SC_RUN_TO_TIME);

	  // Open VCD file
	  sc_trace_file *wf = sc_create_vcd_trace_file("counter");
	  // Dump the desired signals
	  sc_trace(wf, clock, "clock");
	  sc_trace(wf, reset, "reset");
	  sc_trace(wf, enable, "enable");
	  sc_trace(wf, counter_out, "count");

	  // Initialize all variables
	  reset = 0;       // initial value of reset
	  enable = 0;      // initial value of enable
	  for (i=0;i<5;i++) {
	    clock = 0;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	    clock = 1;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	  }
	  reset = 1;    // Assert the reset
	  cout << "@" << sc_time_stamp() <<" Asserting reset\n" << endl;
	  for (i=0;i<10;i++) {
	    clock = 0;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	    clock = 1;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	  }
	  reset = 0;    // De-assert the reset
	  cout << "@" << sc_time_stamp() <<" De-Asserting reset\n" << endl;
	  for (i=0;i<5;i++) {
	    clock = 0;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	    clock = 1;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	  }
	  cout << "@" << sc_time_stamp() <<" Asserting Enable\n" << endl;
	  enable = 1;  // Assert enable
	  for (i=0;i<20;i++) {
	    clock = 0;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	    clock = 1;
	    sc_start(1,SC_NS,SC_RUN_TO_TIME);
	  }
	  cout << "@" << sc_time_stamp() <<" De-Asserting Enable\n" << endl;
	  enable = 0; // De-assert enable

	  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
	  sc_close_vcd_trace_file(wf);
	  return 0;// Terminate simulation
}

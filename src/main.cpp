// All systemc modules should include systemc.h header file
#include "systemc.h"
#include "tlm.h"

#include "Encoder.h"

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
  hello_world hello("HELLO");
  sc_signal<bool> clk;
  sc_signal<bool> rst;
  sc_signal<bool> ena;
  sc_signal<sc_uint<4>,SC_MANY_WRITERS > cont_out{"out"};
  //sc_start();
  //prepare the simulation
  // Print the hello world
  hello.say_hello();
  //UP_COUNTER_WITH_LOAD counter1 (0);
  first_counter counterExample("testCounter");
  counterExample.clock(clk);
  counterExample.reset(rst);
  counterExample.enable(ena);
  counterExample.counter_out(cont_out);
  //sc_signal<sc_uint<4>, > cont_out{"hello"};
  //cout <<"got here";
  sc_start();
  //cout <<"got here2";
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("counter");
  // Dump the desired signals
  //cout <<"got here3";
  sc_trace(wf, clk, "clock");
  sc_trace(wf, rst, "reset");
  sc_trace(wf, ena, "enable");
  sc_trace(wf, cont_out, "count");
  //cout <<"got here4";

  rst = 0;
  ena = 1;
  for (int i=0;i<5;i++){
	  clk = 0;
	  sc_start();
	  clk = 1;
	  sc_start();
  }
  //cout <<"got here5";
  rst =1;
  cout <<"assert rst"<< sc_time_stamp()<<""<<endl;

  sc_close_vcd_trace_file(wf);

  //counter1.counter();

  return(0);
}


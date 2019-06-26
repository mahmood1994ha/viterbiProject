// All systemc modules should include systemc.h header file
#include "systemc.h"
#include "tlm.h"

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

/*SC_MODULE(UP_COUNTER_WITH_LOAD){
	//define in ports
	sc_in <sc_uint<16> > delta;
	sc_in <bool> load, enable, clk, rst;
	//output ports
	sc_out <sc_uint<16> > output;
	//internal attributes
	sc_uint<16> count;

	//define functionality

	void counter(){
		if (rst.read()){
			count = 0;
		}else if (enable.read()){
			if (load.read()){
				count = delta.read();
			} else {
				count = count +1;
			}
		}
		output.write(count);
	}

	SC_CTOR(UP_COUNTER_WITH_LOAD){
		SC_METHOD(counter);
		sensitive << clk.pos();
	}
};*/
//----------------------------------------
SC_MODULE (first_counter) {
  sc_in_clk     clock ;      // Clock input of the design
  sc_in<bool>   reset ;      // active high, synchronous Reset input
  sc_in<bool>   enable;      // Active high enable signal for counter
  sc_out<sc_uint<4> > counter_out{"out"}; // 4 bit vector output of the counter

  //------------Local Variables Here---------------------
  sc_uint<4>	count;

  //------------Code Starts Here-------------------------
  // Below function implements actual counter logic
  void incr_count () {
    // At every rising edge of clock we check if reset is active
    // If active, we load the counter output with 4'b0000
    if (reset.read() == 1) {
      count =  0;
      counter_out.write(count);
    // If enable is active, then we increment the counter
    } else if (enable.read() == 1) {
      count = count + 1;
      counter_out.write(count);
      cout<<"@" << sc_time_stamp() <<" :: Incremented Counter "
        <<counter_out.read()<<endl;
    }
  } // End of function incr_count

  // Constructor for the counter
  // Since this counter is a positive edge trigged one,
  // We trigger the below block with respect to positive
  // edge of the clock and also when ever reset changes state
  SC_CTOR(first_counter) {
    cout<<"Executing new"<<endl;
    SC_METHOD(incr_count);
    sensitive << reset;
    sensitive << clock.pos();
  } // End of Constructor

}; // End of Module counter



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
  //sc_start();
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


// thisinterfacemodule communicates between center memory controller
// and coherent manager
// in essense, it will be inform coherent manager when to start
// monitoring an address

`include "assert_macros.svh"
interface coherency_configure_if 
(
	input clk,
	input rst_n
);

	import coherency_ctrl_pkg::*;

	// ack means that the configuration interface has embraced the change.
	logic ack;

	// valid is a typical valid interface
	logic valid;

	// base address denotes the base address to watch
	addr_t base_addr;

	// size denotes how many cache lines we need to watch
	// they should space out to cache lines instead of adjacent bytes
	size_t size;

	// 

	modport master (output valid, base_addr, size,
					input ack);

	modport slave (input valid, base_addr, size,
					output ack);

	`fpv_ready_valid_if(valid, ack, {base_addr, size})


endinterface : coherency_configure_if
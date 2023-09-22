// this is for the decoupled valid ack interface
// valid must assert *before* ack, then ack could assert
// data and valid should not change before ack assertion
interface decoupled_va_if
	#(parameter int unsigned DataWidth = 32)
		(
			input logic clk,
			input logic rst_n
		);

	logic                 valid;
	logic                 ack;
	logic [DataWidth-1:0] data ;

	modport master (
		output valid,  data,
		input ack
	);

	modport slave (
		input valid,  data,
		output ack
	);

	`ifndef SYNTHESIS
		`fpv_valid_ack_if(valid, ack, data)	
	`endif


endinterface : decoupled_va_if
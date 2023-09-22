interface decoupled_v_if #(
	parameter int unsigned DataWidth
)
(
	input clk,
	input rst_n
);
	

	logic valid;
	logic [DataWidth-1: 0] data;


	modport master (output valid, data);
	
	modport slave (input valid, data);

endinterface : decoupled_v_if
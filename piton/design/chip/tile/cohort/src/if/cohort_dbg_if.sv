interface cohort_dbg_if #(parameter int unsigned RegNum = 1) (
	input logic clk,
	input logic rst_n
);

	logic [31:0] dbg_data [RegNum-1:0];

	modport master (output dbg_data);
	modport slave (input dbg_data);

endinterface : cohort_dbg_if
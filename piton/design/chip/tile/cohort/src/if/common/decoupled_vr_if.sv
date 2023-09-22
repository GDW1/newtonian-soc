// the ready valid decoupled interface
// can optionally decide on data width
// note that vr means that valid *should*
// be high first, but it is not mandatory
// it is, however, mandatory that valid
// should not depend on valid
interface decoupled_vr_if
	#(parameter int unsigned DataWidth = fifo_ctrl_pkg::data_width)
		(
			input logic clk,
			input logic rst_n
		);

	logic                 valid;
	logic                 ready;
	logic [DataWidth-1:0] data ;

	modport master (
		output valid,  data,
		input ready
	);

	modport slave (
		input valid,  data,
		output ready
	);

	`ifndef SYNTHESIS
//		`fpv_ready_valid_if(valid, ready, data)
	`endif


endinterface : decoupled_vr_if
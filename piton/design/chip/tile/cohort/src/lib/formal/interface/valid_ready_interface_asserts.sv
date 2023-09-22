`include "assert_macros.svh"
module valid_ready_interface_asserts #(
	parameter DataWidth = -1,
	parameter ModuleName = "ThisName",
	parameter IsAssume = 1
)(
	input logic clk,    // Clock
	input logic rst_n,  // Asynchronous reset active low
	input logic valid,
	input logic ready,
	input logic [DataWidth-1: 0] data
);
generate
	if (IsAssume) begin
		assert_no_change_before_ready: `assert_clk_xrst(valid & !ready -> ##1 !$changed({valid, data}), {"assert data and valid should not chnage once valid if not readyed in", ModuleName})
	end
	else begin
		assume_no_change_before_ready: `assume_clk_xrst(valid & !ready -> ##1 !$changed({valid, data}), {"assume data and valid should not chnage once valid if not readyed in", ModuleName})
	end
endgenerate

genvar i;
generate
	for (int i = 0; i < 2; i++) begin
		`cover_clk_xrst(valid == i)
		`cover_clk_xrst(ready == i)
	end
endgenerate

endmodule : valid_ready_interface_asserts
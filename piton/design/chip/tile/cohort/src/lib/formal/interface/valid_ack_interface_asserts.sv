`include "assert_macros.svh"
module valid_ack_interface_asserts #(
	parameter DataWidth = -1,
	parameter ModuleName = "ThisName",
	parameter IsAssume = 1
)(
	input logic clk,    // Clock
	input logic rst_n,  // Asynchronous reset active low
	input logic valid,
	input logic ack,
	input logic [DataWidth-1: 0] data
);
generate
	if (IsAssume) begin
		assert_no_change_before_ack: `assert_clk_xrst(valid & !ack -> ##1 !$changed({valid, data}), {"assert data and valid should not chnage once valid if not acked in", ModuleName})
		assert_idle_after_handshake: `assert_clk_xrst(valid & ack -> ##1 !valid & !ack, {"assert valid and ack should de-assert after handshake in", ModuleName});
	end
	else begin
		assume_no_change_before_ack: `assume_clk_xrst(valid & !ack -> ##1 !$changed({valid, data}), {"assume data and valid should not chnage once valid if not acked in", ModuleName})
		assume_idle_after_handshake: `assume_clk_xrst(valid & ack -> ##1 !valid & !ack, {"assume valid and ack should de-assert after handshake in", ModuleName});
	end
endgenerate

genvar i;
generate
	for (int i = 0; i < 2; i++) begin
		`cover_clk_xrst(valid == i)
		`cover_clk_xrst(ready == i)
	end
endgenerate

assert_no_ack_before_valid: `assert_clk_xrst(!valid -> !ack, {"ack should not happen before valid in", ModuleName})
assert_ack_eventually_after_valid: `assert_clk_xrst(valid -> s_eventually(ack), {"should eventually ack if valid", ModuleName})


endmodule : valid_ack_interface_asserts
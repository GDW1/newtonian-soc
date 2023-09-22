`timescale 1ns / 1ps

module add_round_key_inner
(
    input wire clk,
    input wire rst,
    input wire [64-1:0] i_data_in_lower,
    input wire [64-1:0] i_data_in_higher,
    input wire [64-1:0] i_key_in_lower,
    input wire [64-1:0] i_key_in_higher,
    input wire i_valid,
    output wire i_ready,
    output wire [64-1:0] o_data_out_higher,
    output wire [64-1:0] o_data_out_lower,
    output wire o_valid,
    input wire o_ready
);

generate
/*
    Begin module add_round_key
*/
/*
    Wires declared by add_round_key
*/
wire FSM_add_round_key_0_in_ready;
wire [64-1:0] FSM_add_round_key_0_out_data_out_higher;
wire [64-1:0] FSM_add_round_key_0_out_data_out_lower;
wire FSM_add_round_key_0_out_valid;
/* End wires declared by add_round_key */

/*
    Wiring by add_round_key
*/
assign i_ready = FSM_add_round_key_0_in_ready;
assign o_data_out_higher = FSM_add_round_key_0_out_data_out_higher;
assign o_data_out_lower = FSM_add_round_key_0_out_data_out_lower;
assign o_valid = FSM_add_round_key_0_out_valid;
/* End wiring by add_round_key */

/*
    Submodules of add_round_key
*/
reg [64-1:0] FSM_add_round_key_0_st_dummy_reg = 64'b0;

assign FSM_add_round_key_0_out_data_out_higher = i_data_in_higher ^ i_key_in_higher;
assign FSM_add_round_key_0_out_data_out_lower = i_data_in_lower ^ i_key_in_lower;
assign FSM_add_round_key_0_out_valid = 1'b1;

assign FSM_add_round_key_0_in_ready = 1'b1;

always @(posedge clk) begin
    FSM_add_round_key_0_st_dummy_reg <= FSM_add_round_key_0_st_dummy_reg;
    if (rst) begin
        FSM_add_round_key_0_st_dummy_reg <= 64'b0;
    end
end
/* End submodules of add_round_key */
/* End module add_round_key */
endgenerate
endmodule

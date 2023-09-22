`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Gourav saini 
// 
// Create Date:    13:59:56 07/26/2020 
// Design Name: 
// Module Name:    last_round 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module last_round_visc(clk,data_in,key_in,data_out_last);
input clk;
input [127:0]data_in;
input [127:0]key_in;
output [127:0] data_out_last;

wire [127:0] sub_data_out,shift_data_out;

sub_bytes_inner a1(
    .clk(clk),
    .rst(1'b0),
    .i_higher_bits(data_in[127:64]),
    .i_lower_bits(data_in[63:0]),
    .o_higher_out(sub_data_out[127:64]),
    .o_lower_out(sub_data_out[63:0])
);


shift_rows_inner a2(
    .clk(clk),
    .rst(1'b0),
    .i_higher_bits(sub_data_out[127:64]),
    .i_lower_bits(sub_data_out[63:0]),
    .o_data_out_higher(shift_data_out[127:64]),
    .o_data_out_lower(shift_data_out[63:0])
);

assign data_out_last=shift_data_out^key_in;


endmodule



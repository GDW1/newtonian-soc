module round_visc(clk,data_in,key_in,data_out);
input clk;
input [127:0]data_in,key_in;
output [127:0] data_out;

wire [127:0]sub_data_out,shift_data_out,mix_data_out; 

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

mix_columns_inner a3(
    .clk(clk),
    .rst(1'b0),
    .i_data_higher(shift_data_out[127:64]),
    .i_data_lower(shift_data_out[63:0]),
    .o_data_out_higher(mix_data_out[127:64]),
    .o_data_out_lower(mix_data_out[63:0])
);

assign data_out=mix_data_out^key_in;
endmodule

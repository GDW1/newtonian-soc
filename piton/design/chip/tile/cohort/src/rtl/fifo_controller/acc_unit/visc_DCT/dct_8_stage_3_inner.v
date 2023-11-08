`timescale 1ns / 1ps

module dct_8_stage_3_inner
(
    input wire clk,
    input wire rst,
    input wire [512-1:0] i_data_in,
    input wire i_valid,
    output wire i_ready,
    output wire [512-1:0] o_data_out,
    output wire o_valid,
    input wire o_ready
);

generate
/*
    Begin module dct_8_stage_3
*/
/*
    Wires declared by dct_8_stage_3
*/
wire FSM_dct_8_stage_3_0_in_ready;
wire FSM_dct_8_stage_3_0_out_valid;
/* End wires declared by dct_8_stage_3 */

/*
    Submodules of dct_8_stage_3
*/
reg [64-1:0] FSM_dct_8_stage_3_0_st_dummy_reg = 64'b0;

reg [64-1:0] FSM_dct_8_stage_3_0_t0;
reg [3-1:0] FSM_dct_8_stage_3_0_t1;
reg [64-1:0] FSM_dct_8_stage_3_0_t2;
reg [3-1:0] FSM_dct_8_stage_3_0_t3;
reg [64-1:0] FSM_dct_8_stage_3_0_t4;
reg [512-1:0] FSM_dct_8_stage_3_0_t5;
reg [64-1:0] FSM_dct_8_stage_3_0_t6;
reg [3-1:0] FSM_dct_8_stage_3_0_t7;
reg [64-1:0] FSM_dct_8_stage_3_0_t8;
reg [3-1:0] FSM_dct_8_stage_3_0_t9;
reg [64-1:0] FSM_dct_8_stage_3_0_t10;
reg [512-1:0] FSM_dct_8_stage_3_0_t11;
reg [64-1:0] FSM_dct_8_stage_3_0_t12;
reg [3-1:0] FSM_dct_8_stage_3_0_t13;
reg [64-1:0] FSM_dct_8_stage_3_0_t14;
reg [3-1:0] FSM_dct_8_stage_3_0_t15;
reg [64-1:0] FSM_dct_8_stage_3_0_t16;
reg [64-1:0] FSM_dct_8_stage_3_0_t17;
reg [3-1:0] FSM_dct_8_stage_3_0_t18;
reg [64-1:0] FSM_dct_8_stage_3_0_t19;
reg [65-1:0] FSM_dct_8_stage_3_0_t20;
reg [64-1:0] FSM_dct_8_stage_3_0_t21;
reg [512-1:0] FSM_dct_8_stage_3_0_t22;
reg [64-1:0] FSM_dct_8_stage_3_0_t23;
reg [3-1:0] FSM_dct_8_stage_3_0_t24;
reg [64-1:0] FSM_dct_8_stage_3_0_t25;
reg [3-1:0] FSM_dct_8_stage_3_0_t26;
reg [64-1:0] FSM_dct_8_stage_3_0_t27;
reg [512-1:0] FSM_dct_8_stage_3_0_t28;
reg [64-1:0] FSM_dct_8_stage_3_0_t29;
reg [3-1:0] FSM_dct_8_stage_3_0_t30;
reg [64-1:0] FSM_dct_8_stage_3_0_t31;
reg [3-1:0] FSM_dct_8_stage_3_0_t32;
reg [64-1:0] FSM_dct_8_stage_3_0_t33;
reg [512-1:0] FSM_dct_8_stage_3_0_t34;
reg [64-1:0] FSM_dct_8_stage_3_0_t35;
reg [3-1:0] FSM_dct_8_stage_3_0_t36;
reg [64-1:0] FSM_dct_8_stage_3_0_t37;
reg [3-1:0] FSM_dct_8_stage_3_0_t38;
reg [64-1:0] FSM_dct_8_stage_3_0_t39;
reg [512-1:0] FSM_dct_8_stage_3_0_t40;
reg [64-1:0] FSM_dct_8_stage_3_0_t41;
reg [3-1:0] FSM_dct_8_stage_3_0_t42;
reg [64-1:0] FSM_dct_8_stage_3_0_t43;
reg [3-1:0] FSM_dct_8_stage_3_0_t44;
reg [64-1:0] FSM_dct_8_stage_3_0_t45;
reg [512-1:0] FSM_dct_8_stage_3_0_t46;
reg [64-1:0] FSM_dct_8_stage_3_0_t47;
reg [3-1:0] FSM_dct_8_stage_3_0_t48;
reg [64-1:0] FSM_dct_8_stage_3_0_t49;
reg [3-1:0] FSM_dct_8_stage_3_0_t50;
reg [64-1:0] FSM_dct_8_stage_3_0_t51;
reg [512-1:0] FSM_dct_8_stage_3_0_t52;

/*
    Wiring by dct_8_stage_3
*/
assign i_ready = FSM_dct_8_stage_3_0_in_ready;
assign o_data_out = FSM_dct_8_stage_3_0_t52;
assign o_valid = FSM_dct_8_stage_3_0_out_valid;
/* End wiring by dct_8_stage_3 */

assign FSM_dct_8_stage_3_0_out_valid = 1'b1;

initial begin
    FSM_dct_8_stage_3_0_t0 = 64'b0;
    FSM_dct_8_stage_3_0_t1 = FSM_dct_8_stage_3_0_t0[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t2 = 64'b0;
    FSM_dct_8_stage_3_0_t3 = FSM_dct_8_stage_3_0_t2[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t4 = i_data_in[FSM_dct_8_stage_3_0_t3 * 64 +: 64];
    FSM_dct_8_stage_3_0_t5 = 512'b0;
    FSM_dct_8_stage_3_0_t5[FSM_dct_8_stage_3_0_t1 * 64 +: 64] = FSM_dct_8_stage_3_0_t4;
    FSM_dct_8_stage_3_0_t6 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_3_0_t7 = FSM_dct_8_stage_3_0_t6[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t8 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_3_0_t9 = FSM_dct_8_stage_3_0_t8[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t10 = i_data_in[FSM_dct_8_stage_3_0_t9 * 64 +: 64];
    FSM_dct_8_stage_3_0_t11 = FSM_dct_8_stage_3_0_t5;
    FSM_dct_8_stage_3_0_t11[FSM_dct_8_stage_3_0_t7 * 64 +: 64] = FSM_dct_8_stage_3_0_t10;
    FSM_dct_8_stage_3_0_t12 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_3_0_t13 = FSM_dct_8_stage_3_0_t12[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t14 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_3_0_t15 = FSM_dct_8_stage_3_0_t14[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t16 = i_data_in[FSM_dct_8_stage_3_0_t15 * 64 +: 64];
    FSM_dct_8_stage_3_0_t17 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t18 = FSM_dct_8_stage_3_0_t17[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t19 = i_data_in[FSM_dct_8_stage_3_0_t18 * 64 +: 64];
    FSM_dct_8_stage_3_0_t20 = FSM_dct_8_stage_3_0_t16 + FSM_dct_8_stage_3_0_t19;
    FSM_dct_8_stage_3_0_t21 = FSM_dct_8_stage_3_0_t20[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_3_0_t22 = FSM_dct_8_stage_3_0_t11;
    FSM_dct_8_stage_3_0_t22[FSM_dct_8_stage_3_0_t13 * 64 +: 64] = FSM_dct_8_stage_3_0_t21;
    FSM_dct_8_stage_3_0_t23 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t24 = FSM_dct_8_stage_3_0_t23[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t26 = FSM_dct_8_stage_3_0_t25[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t27 = i_data_in[FSM_dct_8_stage_3_0_t26 * 64 +: 64];
    FSM_dct_8_stage_3_0_t28 = FSM_dct_8_stage_3_0_t22;
    FSM_dct_8_stage_3_0_t28[FSM_dct_8_stage_3_0_t24 * 64 +: 64] = FSM_dct_8_stage_3_0_t27;
    FSM_dct_8_stage_3_0_t29 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_3_0_t30 = FSM_dct_8_stage_3_0_t29[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t31 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_3_0_t32 = FSM_dct_8_stage_3_0_t31[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t33 = i_data_in[FSM_dct_8_stage_3_0_t32 * 64 +: 64];
    FSM_dct_8_stage_3_0_t34 = FSM_dct_8_stage_3_0_t28;
    FSM_dct_8_stage_3_0_t34[FSM_dct_8_stage_3_0_t30 * 64 +: 64] = FSM_dct_8_stage_3_0_t33;
    FSM_dct_8_stage_3_0_t35 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_3_0_t36 = FSM_dct_8_stage_3_0_t35[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t37 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_3_0_t38 = FSM_dct_8_stage_3_0_t37[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t39 = i_data_in[FSM_dct_8_stage_3_0_t38 * 64 +: 64];
    FSM_dct_8_stage_3_0_t40 = FSM_dct_8_stage_3_0_t34;
    FSM_dct_8_stage_3_0_t40[FSM_dct_8_stage_3_0_t36 * 64 +: 64] = FSM_dct_8_stage_3_0_t39;
    FSM_dct_8_stage_3_0_t41 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_3_0_t42 = FSM_dct_8_stage_3_0_t41[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t43 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_3_0_t44 = FSM_dct_8_stage_3_0_t43[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t45 = i_data_in[FSM_dct_8_stage_3_0_t44 * 64 +: 64];
    FSM_dct_8_stage_3_0_t46 = FSM_dct_8_stage_3_0_t40;
    FSM_dct_8_stage_3_0_t46[FSM_dct_8_stage_3_0_t42 * 64 +: 64] = FSM_dct_8_stage_3_0_t45;
    FSM_dct_8_stage_3_0_t47 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_3_0_t48 = FSM_dct_8_stage_3_0_t47[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t49 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_3_0_t50 = FSM_dct_8_stage_3_0_t49[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t51 = i_data_in[FSM_dct_8_stage_3_0_t50 * 64 +: 64];
    FSM_dct_8_stage_3_0_t52 = FSM_dct_8_stage_3_0_t46;
    FSM_dct_8_stage_3_0_t52[FSM_dct_8_stage_3_0_t48 * 64 +: 64] = FSM_dct_8_stage_3_0_t51;
end

always @* begin
    FSM_dct_8_stage_3_0_t0 = 64'b0;
    FSM_dct_8_stage_3_0_t1 = FSM_dct_8_stage_3_0_t0[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t2 = 64'b0;
    FSM_dct_8_stage_3_0_t3 = FSM_dct_8_stage_3_0_t2[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t4 = i_data_in[FSM_dct_8_stage_3_0_t3 * 64 +: 64];
    FSM_dct_8_stage_3_0_t5 = 512'b0;
    FSM_dct_8_stage_3_0_t5[FSM_dct_8_stage_3_0_t1 * 64 +: 64] = FSM_dct_8_stage_3_0_t4;
    FSM_dct_8_stage_3_0_t6 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_3_0_t7 = FSM_dct_8_stage_3_0_t6[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t8 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_3_0_t9 = FSM_dct_8_stage_3_0_t8[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t10 = i_data_in[FSM_dct_8_stage_3_0_t9 * 64 +: 64];
    FSM_dct_8_stage_3_0_t11 = FSM_dct_8_stage_3_0_t5;
    FSM_dct_8_stage_3_0_t11[FSM_dct_8_stage_3_0_t7 * 64 +: 64] = FSM_dct_8_stage_3_0_t10;
    FSM_dct_8_stage_3_0_t12 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_3_0_t13 = FSM_dct_8_stage_3_0_t12[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t14 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_3_0_t15 = FSM_dct_8_stage_3_0_t14[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t16 = i_data_in[FSM_dct_8_stage_3_0_t15 * 64 +: 64];
    FSM_dct_8_stage_3_0_t17 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t18 = FSM_dct_8_stage_3_0_t17[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t19 = i_data_in[FSM_dct_8_stage_3_0_t18 * 64 +: 64];
    FSM_dct_8_stage_3_0_t20 = FSM_dct_8_stage_3_0_t16 + FSM_dct_8_stage_3_0_t19;
    FSM_dct_8_stage_3_0_t21 = FSM_dct_8_stage_3_0_t20[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_3_0_t22 = FSM_dct_8_stage_3_0_t11;
    FSM_dct_8_stage_3_0_t22[FSM_dct_8_stage_3_0_t13 * 64 +: 64] = FSM_dct_8_stage_3_0_t21;
    FSM_dct_8_stage_3_0_t23 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t24 = FSM_dct_8_stage_3_0_t23[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_3_0_t26 = FSM_dct_8_stage_3_0_t25[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t27 = i_data_in[FSM_dct_8_stage_3_0_t26 * 64 +: 64];
    FSM_dct_8_stage_3_0_t28 = FSM_dct_8_stage_3_0_t22;
    FSM_dct_8_stage_3_0_t28[FSM_dct_8_stage_3_0_t24 * 64 +: 64] = FSM_dct_8_stage_3_0_t27;
    FSM_dct_8_stage_3_0_t29 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_3_0_t30 = FSM_dct_8_stage_3_0_t29[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t31 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_3_0_t32 = FSM_dct_8_stage_3_0_t31[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t33 = i_data_in[FSM_dct_8_stage_3_0_t32 * 64 +: 64];
    FSM_dct_8_stage_3_0_t34 = FSM_dct_8_stage_3_0_t28;
    FSM_dct_8_stage_3_0_t34[FSM_dct_8_stage_3_0_t30 * 64 +: 64] = FSM_dct_8_stage_3_0_t33;
    FSM_dct_8_stage_3_0_t35 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_3_0_t36 = FSM_dct_8_stage_3_0_t35[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t37 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_3_0_t38 = FSM_dct_8_stage_3_0_t37[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t39 = i_data_in[FSM_dct_8_stage_3_0_t38 * 64 +: 64];
    FSM_dct_8_stage_3_0_t40 = FSM_dct_8_stage_3_0_t34;
    FSM_dct_8_stage_3_0_t40[FSM_dct_8_stage_3_0_t36 * 64 +: 64] = FSM_dct_8_stage_3_0_t39;
    FSM_dct_8_stage_3_0_t41 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_3_0_t42 = FSM_dct_8_stage_3_0_t41[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t43 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_3_0_t44 = FSM_dct_8_stage_3_0_t43[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t45 = i_data_in[FSM_dct_8_stage_3_0_t44 * 64 +: 64];
    FSM_dct_8_stage_3_0_t46 = FSM_dct_8_stage_3_0_t40;
    FSM_dct_8_stage_3_0_t46[FSM_dct_8_stage_3_0_t42 * 64 +: 64] = FSM_dct_8_stage_3_0_t45;
    FSM_dct_8_stage_3_0_t47 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_3_0_t48 = FSM_dct_8_stage_3_0_t47[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t49 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_3_0_t50 = FSM_dct_8_stage_3_0_t49[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_3_0_t51 = i_data_in[FSM_dct_8_stage_3_0_t50 * 64 +: 64];
    FSM_dct_8_stage_3_0_t52 = FSM_dct_8_stage_3_0_t46;
    FSM_dct_8_stage_3_0_t52[FSM_dct_8_stage_3_0_t48 * 64 +: 64] = FSM_dct_8_stage_3_0_t51;
end

assign FSM_dct_8_stage_3_0_in_ready = 1'b1;

always @(posedge clk) begin
    FSM_dct_8_stage_3_0_st_dummy_reg <= FSM_dct_8_stage_3_0_st_dummy_reg;
    if (rst) begin
        FSM_dct_8_stage_3_0_st_dummy_reg <= 64'b0;
    end
end
/* End submodules of dct_8_stage_3 */
/* End module dct_8_stage_3 */
endgenerate
endmodule

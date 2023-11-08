`timescale 1ns / 1ps

module dct_8_stage_5_inner
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
    Begin module dct_8_stage_5
*/
/*
    Wires declared by dct_8_stage_5
*/
wire FSM_dct_8_stage_5_0_in_ready;
wire FSM_dct_8_stage_5_0_out_valid;
/* End wires declared by dct_8_stage_5 */

/*
    Submodules of dct_8_stage_5
*/
reg [64-1:0] FSM_dct_8_stage_5_0_st_dummy_reg = 64'b0;

reg [64-1:0] FSM_dct_8_stage_5_0_t0;
reg [3-1:0] FSM_dct_8_stage_5_0_t1;
reg [64-1:0] FSM_dct_8_stage_5_0_t2;
reg [3-1:0] FSM_dct_8_stage_5_0_t3;
reg [64-1:0] FSM_dct_8_stage_5_0_t4;
reg [64-1:0] FSM_dct_8_stage_5_0_t5;
reg [3-1:0] FSM_dct_8_stage_5_0_t6;
reg [64-1:0] FSM_dct_8_stage_5_0_t7;
reg [65-1:0] FSM_dct_8_stage_5_0_t8;
reg [64-1:0] FSM_dct_8_stage_5_0_t9;
reg [512-1:0] FSM_dct_8_stage_5_0_t10;
reg [64-1:0] FSM_dct_8_stage_5_0_t11;
reg [3-1:0] FSM_dct_8_stage_5_0_t12;
reg [512-1:0] FSM_dct_8_stage_5_0_t13;
reg [64-1:0] FSM_dct_8_stage_5_0_t14;
reg [3-1:0] FSM_dct_8_stage_5_0_t15;
reg [64-1:0] FSM_dct_8_stage_5_0_t16;
reg [3-1:0] FSM_dct_8_stage_5_0_t17;
reg [64-1:0] FSM_dct_8_stage_5_0_t18;
reg [64-1:0] FSM_dct_8_stage_5_0_t19;
reg [3-1:0] FSM_dct_8_stage_5_0_t20;
reg [64-1:0] FSM_dct_8_stage_5_0_t21;
reg [65-1:0] FSM_dct_8_stage_5_0_t22;
reg [64-1:0] FSM_dct_8_stage_5_0_t23;
reg [512-1:0] FSM_dct_8_stage_5_0_t24;
reg [64-1:0] FSM_dct_8_stage_5_0_t25;
reg [3-1:0] FSM_dct_8_stage_5_0_t26;
reg [512-1:0] FSM_dct_8_stage_5_0_t27;
reg [64-1:0] FSM_dct_8_stage_5_0_t28;
reg [3-1:0] FSM_dct_8_stage_5_0_t29;
reg [64-1:0] FSM_dct_8_stage_5_0_t30;
reg [3-1:0] FSM_dct_8_stage_5_0_t31;
reg [64-1:0] FSM_dct_8_stage_5_0_t32;
reg [64-1:0] FSM_dct_8_stage_5_0_t33;
reg [3-1:0] FSM_dct_8_stage_5_0_t34;
reg [64-1:0] FSM_dct_8_stage_5_0_t35;
reg [64-1:0] FSM_dct_8_stage_5_0_t36;
reg [3-1:0] FSM_dct_8_stage_5_0_t37;
reg [64-1:0] FSM_dct_8_stage_5_0_t38;
reg [65-1:0] FSM_dct_8_stage_5_0_t39;
reg [64-1:0] FSM_dct_8_stage_5_0_t40;
reg [512-1:0] FSM_dct_8_stage_5_0_t41;
reg [64-1:0] FSM_dct_8_stage_5_0_t42;
reg [3-1:0] FSM_dct_8_stage_5_0_t43;
reg [65-1:0] FSM_dct_8_stage_5_0_t44;
reg [64-1:0] FSM_dct_8_stage_5_0_t45;
reg [64-1:0] FSM_dct_8_stage_5_0_t46;
reg [3-1:0] FSM_dct_8_stage_5_0_t47;
reg [64-1:0] FSM_dct_8_stage_5_0_t48;
reg [65-1:0] FSM_dct_8_stage_5_0_t49;
reg [64-1:0] FSM_dct_8_stage_5_0_t50;
reg [512-1:0] FSM_dct_8_stage_5_0_t51;
reg [64-1:0] FSM_dct_8_stage_5_0_t52;
reg [3-1:0] FSM_dct_8_stage_5_0_t53;
reg [512-1:0] FSM_dct_8_stage_5_0_t54;
reg [64-1:0] FSM_dct_8_stage_5_0_t55;
reg [3-1:0] FSM_dct_8_stage_5_0_t56;
reg [512-1:0] FSM_dct_8_stage_5_0_t57;

/*
    Wiring by dct_8_stage_5
*/
assign i_ready = FSM_dct_8_stage_5_0_in_ready;
assign o_data_out = FSM_dct_8_stage_5_0_t57;
assign o_valid = FSM_dct_8_stage_5_0_out_valid;
/* End wiring by dct_8_stage_5 */

assign FSM_dct_8_stage_5_0_out_valid = 1'b1;

initial begin
    FSM_dct_8_stage_5_0_t0 = 64'b0;
    FSM_dct_8_stage_5_0_t1 = FSM_dct_8_stage_5_0_t0[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t2 = 64'b0;
    FSM_dct_8_stage_5_0_t3 = FSM_dct_8_stage_5_0_t2[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t4 = i_data_in[FSM_dct_8_stage_5_0_t3 * 64 +: 64];
    FSM_dct_8_stage_5_0_t5 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_5_0_t6 = FSM_dct_8_stage_5_0_t5[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t7 = i_data_in[FSM_dct_8_stage_5_0_t6 * 64 +: 64];
    FSM_dct_8_stage_5_0_t8 = FSM_dct_8_stage_5_0_t4 + FSM_dct_8_stage_5_0_t7;
    FSM_dct_8_stage_5_0_t9 = FSM_dct_8_stage_5_0_t8[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t10 = 512'b0;
    FSM_dct_8_stage_5_0_t10[FSM_dct_8_stage_5_0_t1 * 64 +: 64] = FSM_dct_8_stage_5_0_t9;
    FSM_dct_8_stage_5_0_t11 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_5_0_t12 = FSM_dct_8_stage_5_0_t11[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t13 = FSM_dct_8_stage_5_0_t10;
    FSM_dct_8_stage_5_0_t13[FSM_dct_8_stage_5_0_t12 * 64 +: 64] = FSM_dct_8_stage_5_0_t4 - FSM_dct_8_stage_5_0_t7;
    FSM_dct_8_stage_5_0_t14 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_5_0_t15 = FSM_dct_8_stage_5_0_t14[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t16 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_5_0_t17 = FSM_dct_8_stage_5_0_t16[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t18 = i_data_in[FSM_dct_8_stage_5_0_t17 * 64 +: 64];
    FSM_dct_8_stage_5_0_t19 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_5_0_t20 = FSM_dct_8_stage_5_0_t19[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t21 = i_data_in[FSM_dct_8_stage_5_0_t20 * 64 +: 64];
    FSM_dct_8_stage_5_0_t22 = FSM_dct_8_stage_5_0_t18 + FSM_dct_8_stage_5_0_t21;
    FSM_dct_8_stage_5_0_t23 = FSM_dct_8_stage_5_0_t22[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t24 = FSM_dct_8_stage_5_0_t13;
    FSM_dct_8_stage_5_0_t24[FSM_dct_8_stage_5_0_t15 * 64 +: 64] = FSM_dct_8_stage_5_0_t23;
    FSM_dct_8_stage_5_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_5_0_t26 = FSM_dct_8_stage_5_0_t25[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t27 = FSM_dct_8_stage_5_0_t24;
    FSM_dct_8_stage_5_0_t27[FSM_dct_8_stage_5_0_t26 * 64 +: 64] = FSM_dct_8_stage_5_0_t18 - FSM_dct_8_stage_5_0_t21;
    FSM_dct_8_stage_5_0_t28 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_5_0_t29 = FSM_dct_8_stage_5_0_t28[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t30 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_5_0_t31 = FSM_dct_8_stage_5_0_t30[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t32 = i_data_in[FSM_dct_8_stage_5_0_t31 * 64 +: 64];
    FSM_dct_8_stage_5_0_t33 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_5_0_t34 = FSM_dct_8_stage_5_0_t33[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t35 = i_data_in[FSM_dct_8_stage_5_0_t34 * 64 +: 64];
    FSM_dct_8_stage_5_0_t36 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_5_0_t37 = FSM_dct_8_stage_5_0_t36[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t38 = i_data_in[FSM_dct_8_stage_5_0_t37 * 64 +: 64];
    FSM_dct_8_stage_5_0_t39 = (FSM_dct_8_stage_5_0_t32 - FSM_dct_8_stage_5_0_t35) + FSM_dct_8_stage_5_0_t38;
    FSM_dct_8_stage_5_0_t40 = FSM_dct_8_stage_5_0_t39[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t41 = FSM_dct_8_stage_5_0_t27;
    FSM_dct_8_stage_5_0_t41[FSM_dct_8_stage_5_0_t29 * 64 +: 64] = FSM_dct_8_stage_5_0_t40;
    FSM_dct_8_stage_5_0_t42 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_5_0_t43 = FSM_dct_8_stage_5_0_t42[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t44 = FSM_dct_8_stage_5_0_t32 + FSM_dct_8_stage_5_0_t35;
    FSM_dct_8_stage_5_0_t45 = FSM_dct_8_stage_5_0_t44[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t46 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_5_0_t47 = FSM_dct_8_stage_5_0_t46[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t48 = i_data_in[FSM_dct_8_stage_5_0_t47 * 64 +: 64];
    FSM_dct_8_stage_5_0_t49 = FSM_dct_8_stage_5_0_t45 + FSM_dct_8_stage_5_0_t48;
    FSM_dct_8_stage_5_0_t50 = FSM_dct_8_stage_5_0_t49[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t51 = FSM_dct_8_stage_5_0_t41;
    FSM_dct_8_stage_5_0_t51[FSM_dct_8_stage_5_0_t43 * 64 +: 64] = FSM_dct_8_stage_5_0_t50;
    FSM_dct_8_stage_5_0_t52 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_5_0_t53 = FSM_dct_8_stage_5_0_t52[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t54 = FSM_dct_8_stage_5_0_t51;
    FSM_dct_8_stage_5_0_t54[FSM_dct_8_stage_5_0_t53 * 64 +: 64] = FSM_dct_8_stage_5_0_t45 - FSM_dct_8_stage_5_0_t48;
    FSM_dct_8_stage_5_0_t55 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_5_0_t56 = FSM_dct_8_stage_5_0_t55[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t57 = FSM_dct_8_stage_5_0_t54;
    FSM_dct_8_stage_5_0_t57[FSM_dct_8_stage_5_0_t56 * 64 +: 64] = (FSM_dct_8_stage_5_0_t32 - FSM_dct_8_stage_5_0_t35) - FSM_dct_8_stage_5_0_t38;
end

always @* begin
    FSM_dct_8_stage_5_0_t0 = 64'b0;
    FSM_dct_8_stage_5_0_t1 = FSM_dct_8_stage_5_0_t0[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t2 = 64'b0;
    FSM_dct_8_stage_5_0_t3 = FSM_dct_8_stage_5_0_t2[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t4 = i_data_in[FSM_dct_8_stage_5_0_t3 * 64 +: 64];
    FSM_dct_8_stage_5_0_t5 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_5_0_t6 = FSM_dct_8_stage_5_0_t5[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t7 = i_data_in[FSM_dct_8_stage_5_0_t6 * 64 +: 64];
    FSM_dct_8_stage_5_0_t8 = FSM_dct_8_stage_5_0_t4 + FSM_dct_8_stage_5_0_t7;
    FSM_dct_8_stage_5_0_t9 = FSM_dct_8_stage_5_0_t8[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t10 = 512'b0;
    FSM_dct_8_stage_5_0_t10[FSM_dct_8_stage_5_0_t1 * 64 +: 64] = FSM_dct_8_stage_5_0_t9;
    FSM_dct_8_stage_5_0_t11 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_5_0_t12 = FSM_dct_8_stage_5_0_t11[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t13 = FSM_dct_8_stage_5_0_t10;
    FSM_dct_8_stage_5_0_t13[FSM_dct_8_stage_5_0_t12 * 64 +: 64] = FSM_dct_8_stage_5_0_t4 - FSM_dct_8_stage_5_0_t7;
    FSM_dct_8_stage_5_0_t14 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_5_0_t15 = FSM_dct_8_stage_5_0_t14[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t16 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_5_0_t17 = FSM_dct_8_stage_5_0_t16[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t18 = i_data_in[FSM_dct_8_stage_5_0_t17 * 64 +: 64];
    FSM_dct_8_stage_5_0_t19 = 64'b0000000000000000000000000000000000000000000000000000000000000010;
    FSM_dct_8_stage_5_0_t20 = FSM_dct_8_stage_5_0_t19[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t21 = i_data_in[FSM_dct_8_stage_5_0_t20 * 64 +: 64];
    FSM_dct_8_stage_5_0_t22 = FSM_dct_8_stage_5_0_t18 + FSM_dct_8_stage_5_0_t21;
    FSM_dct_8_stage_5_0_t23 = FSM_dct_8_stage_5_0_t22[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t24 = FSM_dct_8_stage_5_0_t13;
    FSM_dct_8_stage_5_0_t24[FSM_dct_8_stage_5_0_t15 * 64 +: 64] = FSM_dct_8_stage_5_0_t23;
    FSM_dct_8_stage_5_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_5_0_t26 = FSM_dct_8_stage_5_0_t25[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t27 = FSM_dct_8_stage_5_0_t24;
    FSM_dct_8_stage_5_0_t27[FSM_dct_8_stage_5_0_t26 * 64 +: 64] = FSM_dct_8_stage_5_0_t18 - FSM_dct_8_stage_5_0_t21;
    FSM_dct_8_stage_5_0_t28 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_5_0_t29 = FSM_dct_8_stage_5_0_t28[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t30 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_5_0_t31 = FSM_dct_8_stage_5_0_t30[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t32 = i_data_in[FSM_dct_8_stage_5_0_t31 * 64 +: 64];
    FSM_dct_8_stage_5_0_t33 = 64'b0000000000000000000000000000000000000000000000000000000000000101;
    FSM_dct_8_stage_5_0_t34 = FSM_dct_8_stage_5_0_t33[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t35 = i_data_in[FSM_dct_8_stage_5_0_t34 * 64 +: 64];
    FSM_dct_8_stage_5_0_t36 = 64'b0000000000000000000000000000000000000000000000000000000000000100;
    FSM_dct_8_stage_5_0_t37 = FSM_dct_8_stage_5_0_t36[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t38 = i_data_in[FSM_dct_8_stage_5_0_t37 * 64 +: 64];
    FSM_dct_8_stage_5_0_t39 = (FSM_dct_8_stage_5_0_t32 - FSM_dct_8_stage_5_0_t35) + FSM_dct_8_stage_5_0_t38;
    FSM_dct_8_stage_5_0_t40 = FSM_dct_8_stage_5_0_t39[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t41 = FSM_dct_8_stage_5_0_t27;
    FSM_dct_8_stage_5_0_t41[FSM_dct_8_stage_5_0_t29 * 64 +: 64] = FSM_dct_8_stage_5_0_t40;
    FSM_dct_8_stage_5_0_t42 = 64'b0000000000000000000000000000000000000000000000000000000000000001;
    FSM_dct_8_stage_5_0_t43 = FSM_dct_8_stage_5_0_t42[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t44 = FSM_dct_8_stage_5_0_t32 + FSM_dct_8_stage_5_0_t35;
    FSM_dct_8_stage_5_0_t45 = FSM_dct_8_stage_5_0_t44[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t46 = 64'b0000000000000000000000000000000000000000000000000000000000000110;
    FSM_dct_8_stage_5_0_t47 = FSM_dct_8_stage_5_0_t46[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t48 = i_data_in[FSM_dct_8_stage_5_0_t47 * 64 +: 64];
    FSM_dct_8_stage_5_0_t49 = FSM_dct_8_stage_5_0_t45 + FSM_dct_8_stage_5_0_t48;
    FSM_dct_8_stage_5_0_t50 = FSM_dct_8_stage_5_0_t49[7'b0 * 1 +: 64 * 1];
    FSM_dct_8_stage_5_0_t51 = FSM_dct_8_stage_5_0_t41;
    FSM_dct_8_stage_5_0_t51[FSM_dct_8_stage_5_0_t43 * 64 +: 64] = FSM_dct_8_stage_5_0_t50;
    FSM_dct_8_stage_5_0_t52 = 64'b0000000000000000000000000000000000000000000000000000000000000111;
    FSM_dct_8_stage_5_0_t53 = FSM_dct_8_stage_5_0_t52[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t54 = FSM_dct_8_stage_5_0_t51;
    FSM_dct_8_stage_5_0_t54[FSM_dct_8_stage_5_0_t53 * 64 +: 64] = FSM_dct_8_stage_5_0_t45 - FSM_dct_8_stage_5_0_t48;
    FSM_dct_8_stage_5_0_t55 = 64'b0000000000000000000000000000000000000000000000000000000000000011;
    FSM_dct_8_stage_5_0_t56 = FSM_dct_8_stage_5_0_t55[6'b0 * 1 +: 3 * 1];
    FSM_dct_8_stage_5_0_t57 = FSM_dct_8_stage_5_0_t54;
    FSM_dct_8_stage_5_0_t57[FSM_dct_8_stage_5_0_t56 * 64 +: 64] = (FSM_dct_8_stage_5_0_t32 - FSM_dct_8_stage_5_0_t35) - FSM_dct_8_stage_5_0_t38;
end

assign FSM_dct_8_stage_5_0_in_ready = 1'b1;

always @(posedge clk) begin
    FSM_dct_8_stage_5_0_st_dummy_reg <= FSM_dct_8_stage_5_0_st_dummy_reg;
    if (rst) begin
        FSM_dct_8_stage_5_0_st_dummy_reg <= 64'b0;
    end
end
/* End submodules of dct_8_stage_5 */
/* End module dct_8_stage_5 */
endgenerate
endmodule

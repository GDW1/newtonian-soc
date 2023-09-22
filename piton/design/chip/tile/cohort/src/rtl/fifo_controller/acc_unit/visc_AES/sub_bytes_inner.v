`timescale 1ns / 1ps

module sub_bytes_inner
(
    input wire clk,
    input wire rst,
    input wire [64-1:0] i_higher_bits,
    input wire [64-1:0] i_lower_bits,
    input wire i_valid,
    output wire i_ready,
    output wire [64-1:0] o_higher_out,
    output wire [64-1:0] o_lower_out,
    output wire o_valid,
    input wire o_ready
);

generate
/*
    Begin module sub_bytes
*/
/*
    Wires declared by sub_bytes
*/
wire FSM_sub_bytes_0_in_ready;
wire [64-1:0] FSM_sub_bytes_0_out_higher_out;
wire FSM_sub_bytes_0_out_valid;
/* End wires declared by sub_bytes */


/*
    Submodules of sub_bytes
*/
reg [64-1:0] FSM_sub_bytes_0_st_tmp_out_lower_reg = 64'b0;
reg [64-1:0] FSM_sub_bytes_0_st_temp_out_higher_reg = 64'b0;

reg [1024-1:0] FSM_sub_bytes_0_t0;
reg [64-1:0] FSM_sub_bytes_0_t1;
reg [6-1:0] FSM_sub_bytes_0_t2;
reg [64-1:0] FSM_sub_bytes_0_t3;
reg [6-1:0] FSM_sub_bytes_0_t4;
reg [64-1:0] FSM_sub_bytes_0_t5;
reg [6-1:0] FSM_sub_bytes_0_t6;
reg [64-1:0] FSM_sub_bytes_0_t7;
reg [6-1:0] FSM_sub_bytes_0_t8;
reg [64-1:0] FSM_sub_bytes_0_t9;
reg [6-1:0] FSM_sub_bytes_0_t10;
reg [64-1:0] FSM_sub_bytes_0_t11;
reg [6-1:0] FSM_sub_bytes_0_t12;
reg [64-1:0] FSM_sub_bytes_0_t13;
reg [6-1:0] FSM_sub_bytes_0_t14;
reg [64-1:0] FSM_sub_bytes_0_t15;
reg [6-1:0] FSM_sub_bytes_0_t16;
reg [64-1:0] FSM_sub_bytes_0_t17;
reg [6-1:0] FSM_sub_bytes_0_t18;
reg [64-1:0] FSM_sub_bytes_0_t19;
reg [6-1:0] FSM_sub_bytes_0_t20;
reg [64-1:0] FSM_sub_bytes_0_t21;
reg [6-1:0] FSM_sub_bytes_0_t22;
reg [64-1:0] FSM_sub_bytes_0_t23;
reg [6-1:0] FSM_sub_bytes_0_t24;
reg [64-1:0] FSM_sub_bytes_0_t25;
reg [6-1:0] FSM_sub_bytes_0_t26;
reg [64-1:0] FSM_sub_bytes_0_t27;
reg [6-1:0] FSM_sub_bytes_0_t28;
reg [64-1:0] FSM_sub_bytes_0_t29;
reg [4-1:0] FSM_sub_bytes_0_t30;
reg [64-1:0] FSM_sub_bytes_0_t31;
reg [64-1:0] FSM_sub_bytes_0_t32;
reg [4-1:0] FSM_sub_bytes_0_t33;
reg [64-1:0] FSM_sub_bytes_0_t34;
reg [64-1:0] FSM_sub_bytes_0_t35;
reg [6-1:0] FSM_sub_bytes_0_t36;
reg [64-1:0] FSM_sub_bytes_0_t37;
reg [4-1:0] FSM_sub_bytes_0_t38;
reg [64-1:0] FSM_sub_bytes_0_t39;
reg [64-1:0] FSM_sub_bytes_0_t40;
reg [6-1:0] FSM_sub_bytes_0_t41;
reg [64-1:0] FSM_sub_bytes_0_t42;
reg [4-1:0] FSM_sub_bytes_0_t43;
reg [64-1:0] FSM_sub_bytes_0_t44;
reg [64-1:0] FSM_sub_bytes_0_t45;
reg [6-1:0] FSM_sub_bytes_0_t46;
reg [64-1:0] FSM_sub_bytes_0_t47;
reg [4-1:0] FSM_sub_bytes_0_t48;
reg [64-1:0] FSM_sub_bytes_0_t49;
reg [64-1:0] FSM_sub_bytes_0_t50;
reg [6-1:0] FSM_sub_bytes_0_t51;
reg [64-1:0] FSM_sub_bytes_0_t52;
reg [4-1:0] FSM_sub_bytes_0_t53;
reg [64-1:0] FSM_sub_bytes_0_t54;
reg [64-1:0] FSM_sub_bytes_0_t55;
reg [6-1:0] FSM_sub_bytes_0_t56;
reg [64-1:0] FSM_sub_bytes_0_t57;
reg [4-1:0] FSM_sub_bytes_0_t58;
reg [64-1:0] FSM_sub_bytes_0_t59;
reg [64-1:0] FSM_sub_bytes_0_t60;
reg [6-1:0] FSM_sub_bytes_0_t61;
reg [64-1:0] FSM_sub_bytes_0_t62;
reg [4-1:0] FSM_sub_bytes_0_t63;
reg [64-1:0] FSM_sub_bytes_0_t64;
reg [64-1:0] FSM_sub_bytes_0_t65;
reg [6-1:0] FSM_sub_bytes_0_t66;

assign FSM_sub_bytes_0_out_higher_out = ((((((FSM_sub_bytes_0_t31 | (FSM_sub_bytes_0_t34 << FSM_sub_bytes_0_t36)) | (FSM_sub_bytes_0_t39 << FSM_sub_bytes_0_t41)) | (FSM_sub_bytes_0_t44 << FSM_sub_bytes_0_t46)) | (FSM_sub_bytes_0_t49 << FSM_sub_bytes_0_t51)) | (FSM_sub_bytes_0_t54 << FSM_sub_bytes_0_t56)) | (FSM_sub_bytes_0_t59 << FSM_sub_bytes_0_t61)) | (FSM_sub_bytes_0_t64 << FSM_sub_bytes_0_t66);
assign FSM_sub_bytes_0_out_valid = 1'b1;

/*
    Wiring by sub_bytes
*/
assign i_ready = FSM_sub_bytes_0_in_ready;
assign o_higher_out = FSM_sub_bytes_0_out_higher_out;
assign o_lower_out = FSM_sub_bytes_0_st_tmp_out_lower_reg;
assign o_valid = FSM_sub_bytes_0_out_valid;
/* End wiring by sub_bytes */


initial begin
    FSM_sub_bytes_0_t0[0 * 64 +: 64] = i_lower_bits & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t1 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t2 = FSM_sub_bytes_0_t1[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[1 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t2) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t3 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t4 = FSM_sub_bytes_0_t3[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[2 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t4) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t5 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t6 = FSM_sub_bytes_0_t5[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[3 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t6) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t7 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t8 = FSM_sub_bytes_0_t7[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[4 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t8) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t9 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t10 = FSM_sub_bytes_0_t9[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[5 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t10) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t11 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t12 = FSM_sub_bytes_0_t11[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[6 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t12) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t13 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t14 = FSM_sub_bytes_0_t13[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[7 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t14) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t0[8 * 64 +: 64] = i_higher_bits & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t15 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t16 = FSM_sub_bytes_0_t15[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[9 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t16) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t17 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t18 = FSM_sub_bytes_0_t17[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[10 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t18) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t19 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t20 = FSM_sub_bytes_0_t19[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[11 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t20) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t21 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t22 = FSM_sub_bytes_0_t21[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[12 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t22) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t23 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t24 = FSM_sub_bytes_0_t23[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[13 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t24) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t26 = FSM_sub_bytes_0_t25[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[14 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t26) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t27 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t28 = FSM_sub_bytes_0_t27[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[15 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t28) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t29 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t30 = FSM_sub_bytes_0_t29[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t31 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t30 * 64 +: 64];
    FSM_sub_bytes_0_t32 = 64'b0000000000000000000000000000000000000000000000000000000000001001;
    FSM_sub_bytes_0_t33 = FSM_sub_bytes_0_t32[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t34 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t33 * 64 +: 64];
    FSM_sub_bytes_0_t35 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t36 = FSM_sub_bytes_0_t35[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t37 = 64'b0000000000000000000000000000000000000000000000000000000000001010;
    FSM_sub_bytes_0_t38 = FSM_sub_bytes_0_t37[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t39 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t38 * 64 +: 64];
    FSM_sub_bytes_0_t40 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t41 = FSM_sub_bytes_0_t40[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t42 = 64'b0000000000000000000000000000000000000000000000000000000000001011;
    FSM_sub_bytes_0_t43 = FSM_sub_bytes_0_t42[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t44 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t43 * 64 +: 64];
    FSM_sub_bytes_0_t45 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t46 = FSM_sub_bytes_0_t45[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t47 = 64'b0000000000000000000000000000000000000000000000000000000000001100;
    FSM_sub_bytes_0_t48 = FSM_sub_bytes_0_t47[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t49 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t48 * 64 +: 64];
    FSM_sub_bytes_0_t50 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t51 = FSM_sub_bytes_0_t50[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t52 = 64'b0000000000000000000000000000000000000000000000000000000000001101;
    FSM_sub_bytes_0_t53 = FSM_sub_bytes_0_t52[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t54 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t53 * 64 +: 64];
    FSM_sub_bytes_0_t55 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t56 = FSM_sub_bytes_0_t55[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t57 = 64'b0000000000000000000000000000000000000000000000000000000000001110;
    FSM_sub_bytes_0_t58 = FSM_sub_bytes_0_t57[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t59 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t58 * 64 +: 64];
    FSM_sub_bytes_0_t60 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t61 = FSM_sub_bytes_0_t60[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t62 = 64'b0000000000000000000000000000000000000000000000000000000000001111;
    FSM_sub_bytes_0_t63 = FSM_sub_bytes_0_t62[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t64 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t63 * 64 +: 64];
    FSM_sub_bytes_0_t65 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t66 = FSM_sub_bytes_0_t65[6'b0 * 1 +: 6 * 1];
end

always @* begin
    FSM_sub_bytes_0_t0[0 * 64 +: 64] = i_lower_bits & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t1 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t2 = FSM_sub_bytes_0_t1[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[1 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t2) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t3 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t4 = FSM_sub_bytes_0_t3[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[2 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t4) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t5 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t6 = FSM_sub_bytes_0_t5[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[3 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t6) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t7 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t8 = FSM_sub_bytes_0_t7[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[4 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t8) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t9 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t10 = FSM_sub_bytes_0_t9[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[5 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t10) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t11 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t12 = FSM_sub_bytes_0_t11[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[6 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t12) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t13 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t14 = FSM_sub_bytes_0_t13[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[7 * 64 +: 64] = (i_lower_bits >> FSM_sub_bytes_0_t14) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t0[8 * 64 +: 64] = i_higher_bits & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t15 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t16 = FSM_sub_bytes_0_t15[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[9 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t16) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t17 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t18 = FSM_sub_bytes_0_t17[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[10 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t18) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t19 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t20 = FSM_sub_bytes_0_t19[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[11 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t20) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t21 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t22 = FSM_sub_bytes_0_t21[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[12 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t22) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t23 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t24 = FSM_sub_bytes_0_t23[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[13 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t24) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t25 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t26 = FSM_sub_bytes_0_t25[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[14 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t26) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t27 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t28 = FSM_sub_bytes_0_t27[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t0[15 * 64 +: 64] = (i_higher_bits >> FSM_sub_bytes_0_t28) & 64'b0000000000000000000000000000000000000000000000000000000011111111;
    FSM_sub_bytes_0_t29 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t30 = FSM_sub_bytes_0_t29[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t31 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t30 * 64 +: 64];
    FSM_sub_bytes_0_t32 = 64'b0000000000000000000000000000000000000000000000000000000000001001;
    FSM_sub_bytes_0_t33 = FSM_sub_bytes_0_t32[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t34 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t33 * 64 +: 64];
    FSM_sub_bytes_0_t35 = 64'b0000000000000000000000000000000000000000000000000000000000001000;
    FSM_sub_bytes_0_t36 = FSM_sub_bytes_0_t35[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t37 = 64'b0000000000000000000000000000000000000000000000000000000000001010;
    FSM_sub_bytes_0_t38 = FSM_sub_bytes_0_t37[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t39 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t38 * 64 +: 64];
    FSM_sub_bytes_0_t40 = 64'b0000000000000000000000000000000000000000000000000000000000010000;
    FSM_sub_bytes_0_t41 = FSM_sub_bytes_0_t40[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t42 = 64'b0000000000000000000000000000000000000000000000000000000000001011;
    FSM_sub_bytes_0_t43 = FSM_sub_bytes_0_t42[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t44 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t43 * 64 +: 64];
    FSM_sub_bytes_0_t45 = 64'b0000000000000000000000000000000000000000000000000000000000011000;
    FSM_sub_bytes_0_t46 = FSM_sub_bytes_0_t45[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t47 = 64'b0000000000000000000000000000000000000000000000000000000000001100;
    FSM_sub_bytes_0_t48 = FSM_sub_bytes_0_t47[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t49 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t48 * 64 +: 64];
    FSM_sub_bytes_0_t50 = 64'b0000000000000000000000000000000000000000000000000000000000100000;
    FSM_sub_bytes_0_t51 = FSM_sub_bytes_0_t50[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t52 = 64'b0000000000000000000000000000000000000000000000000000000000001101;
    FSM_sub_bytes_0_t53 = FSM_sub_bytes_0_t52[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t54 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t53 * 64 +: 64];
    FSM_sub_bytes_0_t55 = 64'b0000000000000000000000000000000000000000000000000000000000101000;
    FSM_sub_bytes_0_t56 = FSM_sub_bytes_0_t55[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t57 = 64'b0000000000000000000000000000000000000000000000000000000000001110;
    FSM_sub_bytes_0_t58 = FSM_sub_bytes_0_t57[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t59 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t58 * 64 +: 64];
    FSM_sub_bytes_0_t60 = 64'b0000000000000000000000000000000000000000000000000000000000110000;
    FSM_sub_bytes_0_t61 = FSM_sub_bytes_0_t60[6'b0 * 1 +: 6 * 1];
    FSM_sub_bytes_0_t62 = 64'b0000000000000000000000000000000000000000000000000000000000001111;
    FSM_sub_bytes_0_t63 = FSM_sub_bytes_0_t62[6'b0 * 1 +: 4 * 1];
    FSM_sub_bytes_0_t64 = FSM_sub_bytes_0_t0[FSM_sub_bytes_0_t63 * 64 +: 64];
    FSM_sub_bytes_0_t65 = 64'b0000000000000000000000000000000000000000000000000000000000111000;
    FSM_sub_bytes_0_t66 = FSM_sub_bytes_0_t65[6'b0 * 1 +: 6 * 1];
end

assign FSM_sub_bytes_0_in_ready = 1'b1;

always @(posedge clk) begin
    FSM_sub_bytes_0_st_tmp_out_lower_reg <= FSM_sub_bytes_0_st_tmp_out_lower_reg;
    FSM_sub_bytes_0_st_temp_out_higher_reg <= FSM_sub_bytes_0_st_temp_out_higher_reg;
    if (rst) begin
        FSM_sub_bytes_0_st_tmp_out_lower_reg <= 64'b0;
        FSM_sub_bytes_0_st_temp_out_higher_reg <= 64'b0;
    end
end
/* End submodules of sub_bytes */
/* End module sub_bytes */
endgenerate
endmodule

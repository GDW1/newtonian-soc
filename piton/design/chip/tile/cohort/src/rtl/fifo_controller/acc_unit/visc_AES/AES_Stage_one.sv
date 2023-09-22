module AES_Stage_One_Visc(
        input  logic[127:0] key_in,
        input  logic[127:0] data_in,
        output logic[127:0] data_out,

        input  logic clk,
        input  logic rst_n,

        input  logic ready_in,

        output logic valid_out,

        // one output for each key and one for the output data
        output logic[127:0] key_s2_out,
        output logic[127:0] key_s3_out,
        output logic[127:0] key_s4_out,
        output logic[127:0] key_s5_out,
        output logic[127:0] key_s6_out,
        output logic[127:0] key_s7_out,
        output logic[127:0] key_s8_out,
        output logic[127:0] key_s9_out
    );


// FSM which counts 10 cyclers after the ready signal is set and then sets the valid signal

typedef enum logic[1:0] {S_IDLE, S_COMPUTE, S_FINISHED} state_t;
state_t state, next_state;

logic[$clog2(10)-1:0] counter, next_counter;

always_ff @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        state <= S_IDLE;
        counter <= 0;
    end
    else
    begin
        state <= next_state;
        counter <= next_counter;
    end
end

always_comb begin
    next_state = state;
    case (state) 
        S_IDLE: begin
            if(ready_in) begin
                next_state = S_COMPUTE;
                next_counter = 0;
            end
        end
        S_COMPUTE: begin
            if(counter == 9) begin
                next_state = S_FINISHED;
                next_counter = 0;
            end
            else begin
                next_state = S_COMPUTE;
                next_counter = counter + 1;
            end
        end
        S_FINISHED: begin
            next_state = S_IDLE;
            next_counter = 0;
        end
        default: begin
            next_state = S_IDLE;
            next_counter = 0;
        end
    endcase
end

assign valid_out = (state == S_FINISHED);

// key expansion
wire [127:0] key_s,key_s0,key_s1,key_s2,key_s3,key_s4,key_s5,key_s6,key_s7,key_s8,key_s9;
wire [127:0]r_data_out,r0_data_out,r1_data_out;
wire [704-1:0] o_keys_out_higher;
wire [704-1:0] o_keys_out_lower;


assign r_data_out=data_in^key_s;

key_expansion_inner a0(
    .clk(clk),
    .i_key_lower(key_in[63:0]),
    .i_key_higher(key_in[127:64]),
    .o_keys_out_higher(o_keys_out_higher),
    .o_keys_out_lower(o_keys_out_lower)
);

assign key_s = {o_keys_out_higher[63:0]   , o_keys_out_lower[63:0]    };
assign key_s0 = {o_keys_out_higher[127:64] , o_keys_out_lower[127:64] };
assign key_s1 = {o_keys_out_higher[191:128], o_keys_out_lower[191:128]};
assign key_s2 = {o_keys_out_higher[255:192], o_keys_out_lower[255:192]};
assign key_s3 = {o_keys_out_higher[319:256], o_keys_out_lower[319:256]};
assign key_s4 = {o_keys_out_higher[383:320], o_keys_out_lower[383:320]};
assign key_s5 = {o_keys_out_higher[447:384], o_keys_out_lower[447:384]};
assign key_s6 = {o_keys_out_higher[511:448], o_keys_out_lower[511:448]};
assign key_s7 = {o_keys_out_higher[575:512], o_keys_out_lower[575:512]};
assign key_s8 = {o_keys_out_higher[639:576], o_keys_out_lower[639:576]};
assign key_s9 = {o_keys_out_higher[703:640], o_keys_out_lower[703:640]};

round_visc r0(clk,r_data_out,key_s0,r0_data_out);
round_visc r1(clk,r0_data_out,key_s1,r1_data_out);

assign key_s2_out=key_s2;
assign key_s3_out=key_s3;
assign key_s4_out=key_s4;
assign key_s5_out=key_s5;
assign key_s6_out=key_s6;
assign key_s7_out=key_s7;
assign key_s8_out=key_s8;
assign key_s9_out=key_s9;

assign data_out=r1_data_out;
endmodule